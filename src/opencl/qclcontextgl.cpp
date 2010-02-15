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

#include "qclcontextgl.h"
#include "qcl_gl_p.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLContextGL
    \brief The QCLContextGL class represents an OpenCL context that is suitable for use with OpenGL objects.
    \since 4.7
    \ingroup opencl
*/

/*!
    Constructs a new OpenCL context object that is suitable for use
    with OpenGL objects.
*/
QCLContextGL::QCLContextGL()
    : QCLContext()
{
}

/*!
    Destroys this OpenCL context.
*/
QCLContextGL::~QCLContextGL()
{
}

/*!
    Creates an OpenCL memory buffer from the OpenGL buffer object
    \a bufobj, with the specified \a access mode.
*/
QCLBuffer QCLContextGL::createGLBuffer
    (GLuint bufobj, QCLMemoryObject::MemoryFlags access)
{
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateFromGLBuffer
        (id(), flags, bufobj, &error);
    reportError("QCLContextGL::createGLBuffer:", error);
    if (mem)
        return QCLBuffer(this, mem);
    else
        return QCLBuffer();
}

#if QT_VERSION >= 0x040700 || defined(Q_QDOC)

/*!
    \overload

    Creates an OpenCL memory buffer from the OpenGL buffer object
    \a bufobj, with the specified \a access mode.
*/
QCLBuffer QCLContextGL::createGLBuffer
    (QGLBuffer *bufobj, QCLMemoryObject::MemoryFlags access)
{
    if (!bufobj)
        return QCLBuffer();
    return createGLBuffer(GLuint(bufobj->bufferId()), access);
}

#endif

/*!
    Creates a 2D OpenCL image object from the specified \a mipmapLevel,
    OpenGL \a texture object, and \a access mode.

    The \a type must be one of \c{GL_TEXTURE_2D},
    \c{GL_TEXTURE_CUBE_MAP_POSITIVE_X}, \c{GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
    \c{GL_TEXTURE_CUBE_MAP_POSITIVE_Z}, \c{GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
    \c{GL_TEXTURE_CUBE_MAP_NEGATIVE_Y}, \c{GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
    or \c{GL_TEXTURE_RECTANGLE}.  The \a texture does not need to be
    bound to an OpenGL texture target.

    \sa createTexture3D(), createRenderbuffer()
*/
QCLImage2D QCLContextGL::createTexture2D
    (GLenum type, GLuint texture, GLint mipmapLevel,
     QCLMemoryObject::MemoryFlags access)
{
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateFromGLTexture2D
        (id(), flags, type, mipmapLevel, texture, &error);
    reportError("QCLContextGL::createGLTexture2D:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    \overload

    Creates a 2D OpenCL image object from the specified OpenGL
    \a texture object, and the \a access mode.  If texture type is
    assumed to be \c{GL_TEXTURE_2D} and the mipmap level is
    assumed to be 0.
*/
QCLImage2D QCLContextGL::createTexture2D
    (GLuint texture, QCLMemoryObject::MemoryFlags access)
{
    return createTexture2D(GL_TEXTURE_2D, texture, 0, access);
}

/*!
    Creates a 3D OpenCL image object from the specified \a mipmapLevel,
    OpenGL \a texture object, and \a access mode.

    The \a type must be \c{GL_TEXTURE_3D}.  The \a texture does not need
    to be bound to an OpenGL texture target.

    \sa createTexture2D()
*/
QCLImage3D QCLContextGL::createTexture3D
    (GLenum type, GLuint texture, GLint mipmapLevel,
     QCLMemoryObject::MemoryFlags access)
{
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateFromGLTexture3D
        (id(), flags, type, mipmapLevel, texture, &error);
    reportError("QCLContextGL::createGLTexture3D:", error);
    if (mem)
        return QCLImage3D(this, mem);
    else
        return QCLImage3D();
}

/*!
    \overload

    Creates a 3D OpenCL image object from the specified OpenGL
    \a texture object, and \a access mode.  If texture type is
    assumed to be \c{GL_TEXTURE_3D} and the mipmap level is
    assumed to be 0.
*/
QCLImage3D QCLContextGL::createTexture3D
    (GLuint texture, QCLMemoryObject::MemoryFlags access)
{
    return createTexture3D(GL_TEXTURE_3D, texture, 0, access);
}

/*!
    Creates a 2D OpenCL image object from the specified OpenGL
    \a renderbuffer object, and the \a access mode.

    \sa createTexture2D()
*/
QCLImage2D QCLContextGL::createRenderbuffer
    (GLuint renderbuffer, QCLMemoryObject::MemoryFlags access)
{
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateFromGLRenderbuffer
        (id(), flags, renderbuffer, &error);
    reportError("QCLContextGL::createGLRenderbuffer:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    \internal
*/
void QCLContextGL::reportError(const char *name, cl_int error)
{
    setLastError(error);
    if (error != CL_SUCCESS)
        qWarning() << name << QCLContext::errorName(error);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS

/*!
    \internal
*/
QCLBuffer QCLContextGL::createGLBuffer
    (QMacCompatGLuint bufobj, QCLMemoryObject::MemoryFlags access)
{
    return createGLBuffer(GLuint(bufobj), access);
}

/*!
    \internal
*/
QCLImage2D QCLContextGL::createTexture2D
    (QMacCompatGLenum type, QMacCompatGLuint texture,
     QMacCompatGLint mipmapLevel, QCLMemoryObject::MemoryFlags access)
{
    return createTexture2D(GLenum(type), GLuint(texture),
                           GLint(mipmapLevel), access);
}

/*!
    \internal
*/
QCLImage2D QCLContextGL::createTexture2D
    (QMacCompatGLuint texture, QCLMemoryObject::MemoryFlags access)
{
    return createTexture2D(GLenum(GL_TEXTURE_2D), GLuint(texture),
                           GLint(0), access);
}

/*!
    \internal
*/
QCLImage3D QCLContextGL::createTexture3D
    (QMacCompatGLenum type, QMacCompatGLuint texture,
     QMacCompatGLint mipmapLevel, QCLMemoryObject::MemoryFlags access)
{
    return createTexture3D(GLenum(type), GLuint(texture),
                           GLint(mipmapLevel), access);
}

/*!
    \internal
*/
QCLImage3D QCLContextGL::createTexture3D
    (QMacCompatGLuint texture, QCLMemoryObject::MemoryFlags access)
{
    return createTexture3D(GLenum(GL_TEXTURE_2D), GLuint(texture),
                           GLint(0), access);
}

/*!
    \internal
*/
QCLImage2D QCLContextGL::createRenderbuffer
    (QMacCompatGLuint renderbuffer, QCLMemoryObject::MemoryFlags access)
{
    return createRenderbuffer(GLuint(renderbuffer), access);
}

#endif

QT_END_NAMESPACE
