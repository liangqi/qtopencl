/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "imagecl.h"
#include <QtCore/qvarlengtharray.h>
#include <QtGui/qcolor.h>

class ImageCLContext
{
public:
    ImageCLContext() : context(0), glContext(0) {}
    ~ImageCLContext();

    void init(bool useGL);

    QCLContext *context;
    QCLContextGL *glContext;
    QCLProgram program;
    QCLKernel mandelbrot;
    QCLKernel colorize;
};

void ImageCLContext::init(bool useGL)
{
    if (context)
        return;

    if (useGL) {
        glContext = new QCLContextGL();
        context = glContext;
        if (!glContext->create())
            return;
    } else {
        context = new QCLContext();
        if (!context->create())
            return;
    }

    program = context->buildProgramFromSourceFile
        (QLatin1String(":/mandelbrot.cl"));
    mandelbrot = program.createKernel("mandelbrot");
    colorize = program.createKernel("colorize");
}

ImageCLContext::~ImageCLContext()
{
    delete context;
}

Q_GLOBAL_STATIC(ImageCLContext, image_context)

ImageCL::ImageCL(int width, int height)
    : Image(width, height)
    , wid(width), ht(height)
    , initialized(false)
{
}

ImageCL::~ImageCL()
{
}

void ImageCL::init(bool useGL)
{
    if (initialized)
        return;
    initialized = true;

    // Initialize the context for GL or non-GL.
    ImageCLContext *ctx = image_context();
    ctx->init(useGL);

    // Create an OpenCL buffer to hold the raw iteration count data.
    // The buffer lives on the OpenCL device and does not need to
    // be accessed by the host.
    data = ctx->context->createBufferDevice
        (wid * ht * sizeof(uint), QCL::ReadWrite);
}

QMetaType::Type ImageCL::precision() const
{
    return QMetaType::Float;
}

GLuint ImageCL::textureId()
{
    init(true);

    ImageCLContext *ctx = image_context();
    if (!textureBuffer.create(ctx->glContext, wid, ht))
        qWarning("Could not create the OpenCL texture to render into.");

    return textureBuffer.textureId();
}

void ImageCL::initialize()
{
    init(false);
}

bool ImageCL::hasOpenCL()
{
    // See if we have a GPU amongst the OpenCL devices.
    return !QCLDevice::devices(QCLDevice::GPU).isEmpty();
}

void ImageCL::generateIterationData
    (int maxIterations, const QRectF &region)
{
    init();

    ImageCLContext *ctx = image_context();
    QCLKernel mandelbrot = ctx->mandelbrot;

    mandelbrot.setGlobalWorkSize(wid, ht);
    QCLEvent event = mandelbrot
        (data, float(region.x()), float(region.y()),
         float(region.width()), float(region.height()),
         wid, ht, maxIterations);
    event.wait();
}

void ImageCL::generateImage
    (QImage *image, int maxIterations, const QRgb *colors)
{
    ImageCLContext *ctx = image_context();
    QCLKernel colorize = ctx->colorize;

    // Upload the color table into a buffer in the device.
    QCLBuffer colorBuffer = ctx->context->createBufferDevice
        (maxIterations * sizeof(float) * 4, QCL::ReadOnly);
    QVarLengthArray<float> floatColors;
    for (int index = 0; index < maxIterations; ++index) {
        QColor color(colors[index]);
        floatColors.append(float(color.redF()));
        floatColors.append(float(color.greenF()));
        floatColors.append(float(color.blueF()));
        floatColors.append(float(color.alphaF()));
    }
    colorBuffer.write(floatColors.constData(),
                      maxIterations * sizeof(float) * 4);

    if (!textureBuffer.textureId()) {
        // Create a buffer for the image in the OpenCL device.
        if (imageBuffer.isNull()) {
            imageBuffer = ctx->context->createImage2DDevice
                (QImage::Format_ARGB32, QSize(wid, ht), QCL::WriteOnly);
        }

        // Execute the "colorize" kernel.
        colorize.setGlobalWorkSize(wid, ht);
        QCLEvent event = colorize
            (data, imageBuffer, colorBuffer, wid, maxIterations);

        // Copy the OpenCL image buffer into the host image.
        imageBuffer.read(image);
    } else {
        // Finish previous GL operations on the texture.
        glFinish();

        // Acquire the GL texture object.
        textureBuffer.acquireGL();

        // Execute the "colorize" kernel.
        colorize.setGlobalWorkSize(wid, ht);
        colorize(data, textureBuffer, colorBuffer, wid, maxIterations);

        // Release the GL texture object and wait for it complete.
        // After the release is complete, the texture can be used by GL.
        textureBuffer.releaseGL();
    }
}
