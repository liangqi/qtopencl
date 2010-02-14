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

#include "imagenative.h"
#include <QtCore/qmath.h>

// Define the floating-point precision to use for calculations.
//typedef double mreal;
typedef float mreal;

ImageNative::ImageNative(int width, int height)
    : Image(width, height)
    , data(width, height, QImage::Format_RGB32)
{
}

ImageNative::~ImageNative()
{
}

// QMetaType::Float for 32-bit IEEE precision.
// QMetaType::Double for 64-bit IEEE precision.
// QMetaType::User for arbitrary precision.
QMetaType::Type ImageNative::precision() const
{
    if (sizeof(mreal) == sizeof(float))
        return QMetaType::Float;
    else
        return QMetaType::Double;
}

// Algorithm from: http://en.wikipedia.org/wiki/Mandelbrot_set
// This generates uint values for each pixel.  The top 16 bits
// contain the iteration count, and the bottom 16 bits contains
// an interpolation value for creating gradients between colors.
void ImageNative::generateIterationData
    (int maxIterations, const QRectF& region)
{
    int width = data.width();
    int height = data.height();
    mreal xstep = region.width() / width;
    mreal ystep = region.height() / height;
    mreal yin = region.y();
    uint *line = reinterpret_cast<uint *>(data.bits());
    int stride = data.bytesPerLine() / sizeof(uint);
    line += stride * (height - 1);  // flip y axis so y = 1 is at top.
    mreal loglogb = log(log(2.0));
    mreal invlog2 = 1.0 / log(2.0);
    for (int ypos = 0; ypos < height; ++ypos, yin += ystep) {
        mreal xin = region.x();
        for (int xpos = 0; xpos < width; ++xpos, xin += xstep) {
            // Find the color to use with the "escape time" algorithm.
            int iteration = 0;
            mreal x = 0;
            mreal y = 0;
            while (iteration < maxIterations) {
                mreal x2 = x * x;
                mreal y2 = y * y;
                if ((x2 + y2) > (2.0f * 2.0f))
                    break;
                mreal xtemp = x2 - y2 + xin;
                y = 2 * x * y + yin;
                x = xtemp;
                ++iteration;
            }
            if (iteration < (maxIterations - 1)) {
                // Use the Normalized Iteration Count Algorithm
                // to compute an interpolation value between two
                // adjacent colors for a continuous tone image.
                // From: http://math.unipa.it/~grim/Jbarrallo.PDF
                mreal v = (loglogb - log(log(sqrt(x * x + y * y)))) * invlog2;
                int fraction = int(v * 65535.0f);
                line[xpos] = (iteration << 16) + fraction;
            } else {
                line[xpos] = (iteration << 16);
            }
        }
        line -= stride;
    }
}

void ImageNative::generateImage
    (QImage *image, int maxIterations, const QRgb *colors)
{
    QRgb *dst = reinterpret_cast<uint *>(image->bits());
    const uint *src = reinterpret_cast<uint *>(data.bits());
    uint count = image->width() * image->height();
    while (count-- > 0) {
        int iteration = (src[0] >> 16);
        if (iteration < (maxIterations - 1)) {
            qreal v = (src[0] & 0xFFFF) / 65535.0f;
            QRgb color1 = colors[iteration];
            QRgb color2 = colors[iteration + 1];
            int red = int((qRed(color2) - qRed(color1)) * v) +
                      qRed(color1);
            int green = int((qGreen(color2) - qGreen(color1)) * v) +
                        qGreen(color1);
            int blue = int((qBlue(color2) - qBlue(color1)) * v) +
                       qBlue(color1);
            dst[0] = qRgb(red, green, blue);
        } else if (iteration < maxIterations) {
            dst[0] = colors[iteration];
        } else {
            dst[0] = qRgb(0, 0, 0);
        }
        ++src;
        ++dst;
    }
}
