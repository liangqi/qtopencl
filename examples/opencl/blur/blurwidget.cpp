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

#include "blurwidget.h"
#include "blurtable.h"
#include <QtCore/qvector.h>
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
    srcImageBuffer = context.createImage2DCopy(img, QCL::ReadOnly);
//! [2]

//! [3]
    tmpImageBuffer = context.createImage2DDevice(QImage::Format_ARGB32, img.size(), QCL::ReadWrite);

    dstImage = QImage(img.size(), QImage::Format_ARGB32);
    dstImageBuffer = context.createImage2DDevice(dstImage.format(), dstImage.size(), QCL::WriteOnly);
//! [3]

//! [4]
    weightsBuffer = context.createVector<float>(100);
    offsetsBuffer = context.createVector<float>(100);
//! [4]

//! [5]
    hgaussian = program.createKernel("hgaussian");
    hgaussian.setGlobalWorkSize(img.size());
    hgaussian.setLocalWorkSize(hgaussian.bestLocalWorkSizeImage2D());

    vgaussian = program.createKernel("vgaussian");
    vgaussian.setGlobalWorkSize(img.size());
    vgaussian.setLocalWorkSize(vgaussian.bestLocalWorkSizeImage2D());
//! [5]

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(50);
}

BlurWidget::~BlurWidget()
{
}

void BlurWidget::paintEvent(QPaintEvent *)
{
    QTime time;
    time.start();

    // Upload the weights and offsets into OpenCL.
//! [6]
    offsetsBuffer.write(blurOffsets[radius], blurSizes[radius]);
    weightsBuffer.write(blurWeights[radius], blurSizes[radius]);
//! [6]

    // Execute the horizontal and vertical Gaussian kernels.
//! [7]
    hgaussian(srcImageBuffer, tmpImageBuffer, weightsBuffer, offsetsBuffer, blurSizes[radius]);
    vgaussian(tmpImageBuffer, dstImageBuffer, weightsBuffer, offsetsBuffer, blurSizes[radius]);
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
