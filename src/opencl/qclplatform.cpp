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

#include "qclplatform.h"
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLPlatform
    \brief The QCLPlatform class represents an OpenCL platform definition.
    \since 4.7
    \ingroup opencl
*/

/*!
    \fn QCLPlatform::QCLPlatform()

    Constructs a default OpenCL platform identifier.
*/

/*!
    \fn QCLPlatform::QCLPlatform(cl_platform_id id)

    Constructs an OpenCL platform identifier that corresponds to the
    native OpenCL value \a id.
*/

/*!
    \fn bool QCLPlatform::isNull() const

    Returns true if this OpenCL platform identifier is null.
*/

static QString qt_cl_platform_string(cl_platform_id id, cl_platform_info name)
{
    size_t size;
    if (clGetPlatformInfo(id, name, 0, 0, &size) != CL_SUCCESS)
        return QString();
    QVarLengthArray<char> buf(size);
    if (clGetPlatformInfo(id, name, size, buf.data(), &size) != CL_SUCCESS)
        return QString();
    return QString::fromLatin1(buf.data());
}

/*!
    Returns the profile that is implemented by this OpenCL platform,
    usually \c FULL_PROFILE or \c EMBEDDED_PROFILE.
*/
QString QCLPlatform::profile() const
{
    return qt_cl_platform_string(m_id, CL_PLATFORM_PROFILE);
}

/*!
    Returns the OpenCL version that is implemented by this OpenCL platform,
    usually something like \c{OpenCL 1.0}.
*/
QString QCLPlatform::version() const
{
    return qt_cl_platform_string(m_id, CL_PLATFORM_VERSION);
}

/*!
    Returns the name of this OpenCL platform.
*/
QString QCLPlatform::name() const
{
    return qt_cl_platform_string(m_id, CL_PLATFORM_NAME);
}

/*!
    Returns the name of the vendor of this OpenCL platform.
*/
QString QCLPlatform::vendor() const
{
    return qt_cl_platform_string(m_id, CL_PLATFORM_VENDOR);
}

/*!
    Returns a list of the extensions supported by this OpenCL platform.
*/
QStringList QCLPlatform::extensions() const
{
    return qt_cl_platform_string(m_id, CL_PLATFORM_EXTENSIONS).simplified().split(QChar(' '));
}

/*!
    \fn cl_platform_id QCLPlatform::id() const

    Returns the native OpenCL platform identifier for this object.
*/

/*!
    Returns a list of all OpenCL platforms that are supported by this host.
*/
QList<QCLPlatform> QCLPlatform::platforms()
{
    cl_uint size;
    if (clGetPlatformIDs(0, 0, &size) != CL_SUCCESS)
        return QList<QCLPlatform>();
    QVarLengthArray<cl_platform_id> buf(size);
    if (clGetPlatformIDs(size, buf.data(), &size) != CL_SUCCESS)
        return QList<QCLPlatform>();
    QList<QCLPlatform> platforms;
    for (int index = 0; index < buf.size(); ++index)
        platforms.append(QCLPlatform(buf[index]));
    return platforms;
}

QT_END_NAMESPACE
