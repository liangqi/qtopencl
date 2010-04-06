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

#include "colorizewidget.h"
#include <QtGui/qpainter.h>

ColorizeWidget::ColorizeWidget(QWidget *parent)
    : QWidget(parent)
    , color(255, 100, 0)
{
    if (!context.create())
        qFatal("Could not create OpenCL context");

    program = context.buildProgramFromSourceFile(QLatin1String(":/colorize.cl"));

    QImage img(QLatin1String(":/qtlogo.png"));
    srcImageBuffer = context.createImage2DCopy(img, QCLMemoryObject::ReadOnly);

    dstImage = QImage(img.size(), QImage::Format_ARGB32);
    dstImageBuffer = context.createImage2DDevice(dstImage.format(), dstImage.size(), QCLMemoryObject::WriteOnly);

    colorize = program.createKernel("colorize");
    colorize.setGlobalWorkSize(img.size());
    colorize.setLocalWorkSize(8, 8);
}

ColorizeWidget::~ColorizeWidget()
{
}

void ColorizeWidget::paintEvent(QPaintEvent *)
{
    colorize(srcImageBuffer, dstImageBuffer, color);
    dstImageBuffer.read(&dstImage);
    QPainter painter(this);
    painter.drawImage(0, 0, dstImage);
}
