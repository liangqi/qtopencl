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

#include "qcltexture2d.h"
#include "qclcontextgl.h"
#include <QtOpenGL/private/qgl_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLTexture2D
    \brief The QCLTexture2D class represents a 2D OpenCL texture object.
    \since 4.7
    \ingroup opencl

    Normally applications render into OpenGL textures by calling
    QCLContextGL::createTexture2D() to wrap an existing texture identifier
    with a QCLImage2D object.  However, some systems do not support
    the OpenCL/OpenGL sharing mechanisms that are needed to make that work.

    QCLTexture2D abstracts the creation and management of \c{GL_RGBA}
    textures so that applications can render into them with OpenCL
    kernels without needing to implement special handling for
    OpenCL implementations that lack sharing.
*/

class QCLTexture2DPrivate
{
public:
    QCLTexture2DPrivate()
        : guard(0)
        , directRender(false)
    {
    }
    ~QCLTexture2DPrivate()
    {
    }

    QGLSharedResourceGuard guard;
    QSize size;
    bool directRender;
};

/*!
    Constructs an uninitialized OpenCL texture object.
*/
QCLTexture2D::QCLTexture2D()
    : QCLImage2D(), d_ptr(new QCLTexture2DPrivate())
{
}

/*!
    Destroys this OpenCL texture object.
*/
QCLTexture2D::~QCLTexture2D()
{
    destroy();
}

/*!
    Constructs an OpenCL texture of \a size in \a context.
    Returns true if the texture was created; false otherwise.

    \sa destroy(), textureId()
*/
bool QCLTexture2D::create(QCLContextGL *context, const QSize& size)
{
    Q_D(QCLTexture2D);
    Q_ASSERT(context && size.width() > 0 && size.height() > 0);
    Q_ASSERT(id() == 0);    // Must not be created already.

    // Create the texture in the GL context.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
#ifdef GL_CLAMP_TO_EDGE
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // If the context supports object sharing, then this is really easy.
    if (context->supportsObjectSharing()) {
        QCLImage2D image = context->createTexture2D
            (GL_TEXTURE_2D, textureId, 0, QCLMemoryObject::WriteOnly);
        if (image.isNull()) {
            glDeleteTextures(1, &textureId);
            return false;
        }
        d->guard.setContext(QGLContext::currentContext());
        d->guard.setId(textureId);
        setId(image.context(), image.id());
        d->size = size;
        d->directRender = true;
        return true;
    }

    // Create a 2D image in the OpenCL device for rendering with OpenCL.
    // TODO: use pixel unpack buffers on the texture if available.
    QCLImage2D image = context->createImage2DDevice
        (QCLImageFormat(QCLImageFormat::Order_RGBA,
                        QCLImageFormat::Type_Normalized_UInt8),
         size, QCLMemoryObject::WriteOnly);
    if (image.isNull()) {
        glDeleteTextures(1, &textureId);
        return false;
    }
    d->guard.setContext(QGLContext::currentContext());
    d->guard.setId(textureId);
    setId(image.context(), image.id());
    d->size = size;
    d->directRender = false;
    return true;
}

/*!
    \fn bool QCLTexture2D::create(QCLContextGL *context, int width, int height)
    \overload

    Constructs an OpenCL texture of size (\a width, \a height)
    in \a context.  Returns true if the texture was created; false otherwise.

    \sa destroy()
*/

/*!
    Destroys this OpenCL texture object.
*/
void QCLTexture2D::destroy()
{
    Q_D(QCLTexture2D);
    setId(0, 0);
    GLuint textureId = d->guard.id();
    if (textureId) {
        QGLShareContextScope scope(d->guard.context());
        glDeleteTextures(1, &textureId);
    }
    d->guard.setContext(0);
    d->guard.setId(0);
    d->size = QSize();
    d->directRender = false;
}

/*!
    Acquires access to this texture so that OpenCL kernels
    can render into it.  OpenGL cannot use the texture until
    releaseGL() is called.

    \sa releaseGL()
*/
void QCLTexture2D::acquireGL()
{
    Q_D(QCLTexture2D);
    if (d->directRender)
        QCLImage2D::acquireGL().wait();
}

/*!
    Releases access to this texture so that OpenGL can use it again.
    The textureId() will also be bound to the current OpenGL context.

    \sa acquireGL()
*/
void QCLTexture2D::releaseGL()
{
    Q_D(QCLTexture2D);
    if (!d->guard.id())
        return;

    // If we are doing direct rendering, then just release the OpenCL object.
    if (d->directRender) {
        QCLImage2D::releaseGL().wait();
        glBindTexture(GL_TEXTURE_2D, d->guard.id());
        return;
    }

    // Wait for the current OpenCL commands to finish.
    context()->commandQueue().marker().wait();

    // Upload the contents of the OpenCL buffer into the texture.
    void *ptr = map(QRect(QPoint(0, 0), d->size), MapReadOnly);
    glBindTexture(GL_TEXTURE_2D, d->guard.id());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    d->size.width(), d->size.height(),
                    GL_RGBA, GL_UNSIGNED_BYTE, ptr);
    unmap(ptr);
}

/*!
    Returns the OpenGL texture identifier for this OpenCL texture object.
*/
GLuint QCLTexture2D::textureId() const
{
    Q_D(const QCLTexture2D);
    return d->guard.id();
}

QT_END_NAMESPACE
