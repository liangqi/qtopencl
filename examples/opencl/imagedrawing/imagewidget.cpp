/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenCL module of the Qt Toolkit.
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

#include "imagewidget.h"
#include <QtGui/qpainter.h>
#include <QtGui/qvector4d.h>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    if (!context.create())
        qFatal("Could not create OpenCL context");

    program = context.buildProgramFromSourceFile(QLatin1String(":/imagedrawing.cl"));

    fillRectWithColor = program.createKernel("fillRectWithColor");
    fillRectWithColor.setLocalWorkSize(8, 8);

    drawImageKernel = program.createKernel("drawImage");
    drawImageKernel.setLocalWorkSize(8, 8);

    flower = context.createImage2DCopy
        (QImage(QLatin1String(":/flower.jpg")), QCL::ReadOnly);
    bg1 = context.createImage2DCopy
        (QImage(QLatin1String(":/bg1.jpg")), QCL::ReadOnly);
    beavis = context.createImage2DCopy
        (QImage(QLatin1String(":/beavis.jpg")), QCL::ReadOnly);
    qtlogo = context.createImage2DCopy
        (QImage(QLatin1String(":/qtlogo.png")), QCL::ReadOnly);
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::paintEvent(QPaintEvent *)
{
    // Create a QCLImage2D object for the window surface.
    QSize wsize = size();
    if (wsize != windowSize)
        surfaceImage = QCLImage2D();
    if (surfaceImage.isNull()) {
        windowSize = wsize;
        surfaceImage = context.createImage2DDevice
            (QImage::Format_RGB32, windowSize, QCL::ReadWrite);
    }

    // Clear to the background color.
    QColor bgcolor = palette().color(backgroundRole());
    fillRect(0, 0, wsize.width(), wsize.height(), bgcolor);

    // Draw the images.
    drawImage(flower, 50, 50, 1.0f);
    drawImage(beavis, 200, 100, 0.9f);
    drawScaledImage(beavis, 550, 150, 250, 250, 0.9f);
    drawImage(bg1, 0, 0, 0.7f);
    drawImage(qtlogo, 200, 350, 1.0f);

    // Draw the window surface image to the actual window.
    QPainter painter(this);
    surfaceImage.drawImage(&painter, QPoint(0, 0));
}

void ImageWidget::fillRect(int x, int y, int width, int height,
                          const QColor& color)
{
    // Round up the global work size so we can process the
    // rectangle in 8x8 local work size units.  The kernel will
    // ignore pixels that are outside the rectangle limits.
    fillRectWithColor.setGlobalWorkSize((width + 7) & ~7, (height + 7) & ~7);
    fillRectWithColor(surfaceImage, x, y, x + width, y + height, color);
}

void ImageWidget::drawImage(const QCLImage2D& image, int x, int y, float opacity)
{
    QRect srcRect = QRect(0, 0, image.width(), image.height());
    QRect dstRect = QRect(x, y, srcRect.width(), srcRect.height());

    // Clamp the draw to the surface extents.
    QRect dstRect2 = dstRect.intersect
        (QRect(0, 0, windowSize.width(), windowSize.height()));
    srcRect.setLeft(srcRect.left() + dstRect2.left() - dstRect.left());
    srcRect.setTop(srcRect.top() + dstRect2.top() - dstRect.top());
    srcRect.setRight(srcRect.right() + dstRect2.right() - dstRect.right());
    srcRect.setBottom(srcRect.bottom() + dstRect2.bottom() - dstRect.bottom());
    dstRect = dstRect2;
    if (srcRect.isEmpty() || dstRect.isEmpty())
        return;

    // Set the global work size to the destination size rounded up to 8.
    drawImageKernel.setGlobalWorkSize
        ((dstRect.width() + 7) & ~7, (dstRect.height() + 7) & ~7);

    // Draw the image.
    QVector4D src(srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height());
    drawImageKernel
        (surfaceImage, surfaceImage, image,
         dstRect.x(), dstRect.y(), dstRect.width(), dstRect.height(),
         src, opacity);
}

void ImageWidget::drawScaledImage
    (const QCLImage2D& image, int x, int y, int width, int height,
     float opacity)
{
    QRectF srcRect = QRectF(0, 0, image.width(), image.height());
    QRect dstRect = QRect(x, y, width, height);

    // Clamp the draw to the surface extents.
    QRect dstRect2 = dstRect.intersect
        (QRect(0, 0, windowSize.width(), windowSize.height()));
    qreal scaleX = srcRect.width() / width;
    qreal scaleY = srcRect.height() / height;
    srcRect.setLeft(srcRect.left() + (dstRect2.left() - dstRect.left()) * scaleX);
    srcRect.setTop(srcRect.top() + (dstRect2.top() - dstRect.top()) * scaleY);
    srcRect.setRight(srcRect.right() + (dstRect2.right() - dstRect.right()) * scaleX);
    srcRect.setBottom(srcRect.bottom() + (dstRect2.bottom() - dstRect.bottom()) * scaleY);
    dstRect = dstRect2;
    if (srcRect.isEmpty() || dstRect.isEmpty())
        return;

    // Set the global work size to the destination size rounded up to 8.
    drawImageKernel.setGlobalWorkSize
        ((dstRect.width() + 7) & ~7, (dstRect.height() + 7) & ~7);

    // Draw the image.
    QVector4D src(srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height());
    drawImageKernel
        (surfaceImage, surfaceImage, image,
         dstRect.x(), dstRect.y(), dstRect.width(), dstRect.height(),
         src, opacity);
}
