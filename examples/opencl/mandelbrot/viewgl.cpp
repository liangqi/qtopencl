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

#include "viewgl.h"
#include "image.h"
#include "palette.h"
#include <QtGui/qpainter.h>
#include <QtCore/qtimer.h>

ViewGL::ViewGL(QWidget *parent)
    : QGLWidget(parent)
{
    setMinimumSize(768, 512);
    setMaximumSize(768, 512);

    palette = new Palette();
    palette->setStandardPalette(Palette::Blue);
    offset = 0.0f;
    step = 0.05f;

    image = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(50);
}

ViewGL::~ViewGL()
{
    delete palette;
    delete image;
}

void ViewGL::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void ViewGL::initializeGL()
{
    image = Image::createImage(768, 512);
    textureId = image->textureId();
    image->generate(200, *palette);
}

void ViewGL::paintGL()
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    static GLfloat const texCoords[] = {
        0, 0, 1, 0, 1, 1, 0, 1
    };

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ViewGL::animate()
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
    updateGL();
}
