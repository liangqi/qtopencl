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

__kernel void fillRectWithColor
    (__write_only image2d_t dstImage,
     int offsetX, int offsetY, int limitX, int limitY,
     float4 color)
{
    int x = get_global_id(0) + offsetX;
    int y = get_global_id(1) + offsetY;
    if (x < limitX && y < limitY)
        write_imagef(dstImage, (int2)(x, y), color);
}

const sampler_t lsamp = CLK_ADDRESS_CLAMP_TO_EDGE |
                        CLK_FILTER_LINEAR;
const sampler_t nsamp = CLK_ADDRESS_CLAMP_TO_EDGE |
                        CLK_FILTER_NEAREST;

__kernel void drawImage
    (__write_only image2d_t dstImage,
     __read_only image2d_t dst2Image,
     __read_only image2d_t srcImage,
     int dstx, int dsty, int dstw, int dsth,
     float4 src, float opacity)
{
    int2 dstPos = (int2)(get_global_id(0) + dstx,
                         get_global_id(1) + dsty);
    float2 srcPos = (float2)(get_global_id(0) * src.z / dstw + src.x,
                             get_global_id(1) * src.w / dsth + src.y);
    if (dstPos.x < dstx || dstPos.x >= (dstx + dstw) ||
            dstPos.y < dsty || dstPos.y >= (dsty + dsth))
        return;     // Undrawn extra pixel due to 8x8 round up.
    float4 scolor = read_imagef(srcImage, lsamp, srcPos);
    float4 dcolor = read_imagef(dst2Image, nsamp, dstPos);
    // Destination is assumed to be RGB, source may be RGBA.
    dcolor = (float4)(dcolor.xyz * (1.0f - scolor.w * opacity) + scolor.xyz * scolor.w * opacity, 1.0f);
    write_imagef(dstImage, dstPos, clamp(dcolor, 0.0f, 1.0f));
}
