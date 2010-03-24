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

#include "pathwidget.h"
#include <QtGui/qpainter.h>

PathWidget::PathWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    if (!context.create())
        qFatal("Could not create OpenCL context");

    program = context.buildProgramFromSourceFile(QLatin1String(":/pathdrawing.cl"));

    fillRectWithColor = program.createKernel("fillRectWithColor");
    fillRectWithColor.setLocalWorkSize(8, 8);
}

PathWidget::~PathWidget()
{
}

void PathWidget::paintEvent(QPaintEvent *)
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

    // Draw a rectangle at 150, 100.
    fillRect(150, 100, 300, 200, Qt::black);

    // Draw the window surface image to the actual window.
    QPainter painter(this);
    surfaceImage.drawImage(&painter, QPoint(0, 0));
}

void PathWidget::fillRect(int x, int y, int width, int height,
                          const QColor& color)
{
    // Round up the global work size so we can process the
    // rectangle in 8x8 local work size units.  The kernel will
    // ignore pixels that are outside the rectangle limits.
    fillRectWithColor.setGlobalWorkSize((width + 7) & ~7, (height + 7) & ~7);
    fillRectWithColor(surfaceImage, x, y, x + width, y + height, color);
}
