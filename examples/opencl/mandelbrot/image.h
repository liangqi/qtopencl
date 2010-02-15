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

#ifndef IMAGE_H
#define IMAGE_H

#include <QtGui/qimage.h>
#include <QtCore/qmetatype.h>
#include <QtOpenGL/qgl.h>

class Palette;

class Image
{
public:
    Image(int width, int height);
    virtual ~Image();

    // QMetaType::Float for 32-bit IEEE precision.
    // QMetaType::Double for 64-bit IEEE precision.
    // QMetaType::User for arbitrary precision.
    virtual QMetaType::Type precision() const = 0;

    QImage image() const { return img; }

    virtual GLuint textureId() { return 0; }
    virtual void initialize() {}

    QRectF region() const { return rgn; }
    void setRegion(const QRectF& value)
        { rgn = value; regionChanged = true; }
    void setRegion(qreal centerx, qreal centery, qreal diameterx);

    void forceUpdate() { regionChanged = true; }

    void generate(int maxIterations, const Palette& palette);

    static Image *createImage(int width, int height);

protected:
    virtual void generateIterationData
        (int maxIterations, const QRectF& region) = 0;
    virtual void generateImage
        (QImage *image, int maxIterations, const QRgb *colors) = 0;

private:
    QImage img;
    QRectF rgn;
    bool regionChanged;
    int lastIterations;
};

#endif
