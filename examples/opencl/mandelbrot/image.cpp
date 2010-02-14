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

#include "image.h"
#include "imagenative.h"
#include "imagecl.h"
#include "palette.h"
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

Image::Image(int width, int height)
    : img(width, height, QImage::Format_RGB32)
    , regionChanged(true)
    , lastIterations(-1)
{
    setRegion(-0.7f, 0.0f, 3.0769f);
}

Image::~Image()
{
}

// Set a region based on its center and diameter in the x direction.
void Image::setRegion(qreal centerx, qreal centery, qreal diameterx)
{
    qreal diametery = diameterx * img.height() / img.width();
    rgn = QRectF(centerx - diameterx * 0.5f,
                 centery - diametery * 0.5f,
                 diameterx, diametery);
    regionChanged = true;
}

Image *Image::createImage(int width, int height)
{
    if (ImageCL::hasOpenCL())
        return new ImageCL(width, height);
    else
        return new ImageNative(width, height);
}

void Image::generate(int maxIterations, const Palette& palette)
{
    Q_ASSERT(maxIterations <= 65535);

    // If the parameters have changed, then regenerate the index data.
    if (regionChanged || maxIterations != lastIterations) {
        QTime time;
        time.start();
        generateIterationData(maxIterations, rgn);
        regionChanged = false;
        lastIterations = maxIterations;
        qDebug() << "Mandelbrot:" << rgn
                 << "iterations =" << maxIterations
                 << "elapsed =" << time.elapsed() << "ms";
    }

    // Colorize the index data to create the final image.
    QVector<QRgb> colors = palette.createTable(maxIterations);
    generateImage(&img, maxIterations, colors.constData());
}
