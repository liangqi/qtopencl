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

#include "blurwidget.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qmath.h>
#include <QtGui/qpainter.h>

BlurWidget::BlurWidget(QWidget *parent)
    : QWidget(parent)
    , radius(1)
    , step(1)
{
//! [1]
    if (!context.create())
        qFatal("Could not create OpenCL context");

    program = context.buildProgramFromSourceFile(QLatin1String(":/blur.cl"));
//! [1]

//! [2]
    QImage img(QLatin1String(":/qtlogo.png"));
    srcImageBuffer = context.createImage2DCopy(img, QCLImage2D::ReadOnly);
//! [2]

//! [3]
    tmpImageBuffer = context.createImage2DDevice(QImage::Format_ARGB32, img.size(), QCLImage2D::ReadWrite);

    dstImage = QImage(img.size(), QImage::Format_ARGB32);
    dstImageBuffer = context.createImage2DDevice(dstImage.format(), dstImage.size(), QCLImage2D::WriteOnly);
//! [3]

//! [4]
    weightsBuffer = context.createBufferDevice(sizeof(float) * 100, QCLBuffer::ReadOnly);
    offsetsBuffer = context.createBufferDevice(sizeof(float) * 100, QCLBuffer::ReadOnly);
//! [4]

//! [5]
    hgaussian = program.createKernel("hgaussian");
    hgaussian.setGlobalWorkSize(img.size());

    vgaussian = program.createKernel("vgaussian");
    vgaussian.setGlobalWorkSize(img.size());
//! [5]

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(50);
}

BlurWidget::~BlurWidget()
{
}

static const qreal Q_2PI = qreal(6.28318530717958647693); // 2*pi

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}

void BlurWidget::paintEvent(QPaintEvent *)
{
    // Calculate the Gaussian blur weights and offsets.
    QVarLengthArray<qreal> components;
    QVarLengthArray<float> offsets;
    QVarLengthArray<float> weights;
    qreal sigma = radius / 1.65;
    qreal sum = 0;
    for (int i = -radius; i <= radius; ++i) {
        qreal value = gaussian(i, sigma);
        components.append(value);
        sum += value;
    }
    for (int i = 0; i < components.size(); ++i)
        components[i] /= sum;
    for (int i = 0; i < components.size() - 1; i += 2) {
        qreal weight = components[i] + components[i + 1];
        qreal offset = i - radius + components[i + 1] / weight;
        offsets.append(offset);
        weights.append(weight);
    }
    // odd size ?
    if (components.size() & 1) {
        offsets.append(radius);
        weights.append(components[components.size() - 1]);
    }

    QTime time;
    time.start();

    // Upload the weights and offsets into OpenCL.
//! [6]
    offsetsBuffer.write(offsets.constData(), sizeof(float) * offsets.size());
    weightsBuffer.write(weights.constData(), sizeof(float) * weights.size());
//! [6]

    // Execute the horizontal and vertical Gaussian kernels.
//! [7]
    hgaussian(srcImageBuffer, tmpImageBuffer, weightsBuffer, offsetsBuffer, weights.size());
    vgaussian(tmpImageBuffer, dstImageBuffer, weightsBuffer, offsetsBuffer, weights.size());
//! [7]

    // Read back the destination image into host memory and draw it.
//! [8]
    dstImageBuffer.read(&dstImage);
    QPainter painter(this);
    painter.drawImage(0, 0, dstImage);
//! [8]

    qWarning("Time to blur with radius %d: %d ms", radius, time.elapsed());
}

void BlurWidget::animate()
{
    radius += step;
    if (radius >= 16 || radius <= 1)
        step = -step;
    update();
}
