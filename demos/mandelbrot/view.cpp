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

#include "view.h"
#include "image.h"
#include "palette.h"
#include <QtGui/qpainter.h>
#include <QtCore/qtimer.h>

View::View(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(768, 512);
    setMaximumSize(768, 512);

    palette = new Palette();
    palette->setStandardPalette(Palette::Blue);
    offset = 0.0f;
    step = 0.05f;

    image = Image::createImage(768, 512);
    image->initialize();
    image->generate(200, *palette);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(50);
}

View::~View()
{
    delete palette;
    delete image;
}

void View::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QImage img = image->image();
    painter.drawImage((width() - img.width()) / 2,
                      (height() - img.height()) / 2,
                      img);
}

void View::animate()
{
    if (step > 0) {
        offset += step;
        if (offset >= 1.0f) {
            offset = 1.0f;
            step = -step;
        }
    } else {
        offset += step;
        if (offset <= 0.0f) {
            offset = 0.0f;
            step = -step;
        }
    }
    palette->setOffset(offset);
    //image->forceUpdate();
    image->generate(200, *palette);
    update();
}
