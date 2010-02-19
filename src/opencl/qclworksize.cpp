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

#include "qclworksize.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLWorkSize
    \brief The QCLWorkSize class defines the size of an item of work for an OpenCL kernel.
    \since 4.7
    \ingroup opencl

    OpenCL work sizes may be single-dimensional, two-dimensional,
    or three-dimensional:

    \code
    QCLWorkSize oned(128);
    QCLWorkSize twod(16, 16);
    QCLWorkSize threed(32, 32, 16);
    ...
    QCLKernel kernel = ...;
    kernel.setGlobalWorkSize(oned);
    kernel.setGlobalWorkSize(twod);
    kernel.setGlobalWorkSize(threed);
    \endcode

    For convenience, QCLKernel::setGlobalWorkSize() and
    QCLKernel::setLocalWorkSize() can be specified by a QCLWorkSize
    object or direct numeric arguments:

    \code
    QCLKernel kernel = ...;
    kernel.setGlobalWorkSize(128);
    kernel.setGlobalWorkSize(16, 16);
    kernel.setGlobalWorkSize(32, 32, 16);
    \endcode

    \sa QCLKernel
*/

/*!
    \fn QCLWorkSize::QCLWorkSize()

    Constructs a default work size consisting of a single dimension
    with width() set to 1.
*/

/*!
    \fn QCLWorkSize::QCLWorkSize(size_t size)

    Constructs a single-dimensional work size with width() set to \a size.
*/

/*!
    \fn QCLWorkSize::QCLWorkSize(size_t width, size_t height)

    Constructs a two-dimensional work size of \a width x \a height.
*/

/*!
    \fn QCLWorkSize::QCLWorkSize(const QSize &size)

    Constructs a two-dimensional work size set to \a size.
*/

/*!
    \fn QCLWorkSize::QCLWorkSize(size_t width, size_t height, size_t depth)

    Constructs a three-dimensional work size of \a width x \a height x
    \a depth.
*/

/*!
    \fn size_t QCLWorkSize::dimensions() const

    Returns the number of dimensions for this work size, 1, 2, or 3.
*/

/*!
    \fn size_t QCLWorkSize::width() const

    Returns the width of this work size specification.
*/

/*!
    \fn size_t QCLWorkSize::height() const

    Returns the height of this work size specification.
*/

/*!
    \fn size_t QCLWorkSize::depth() const

    Returns the depth of this work size specification.
*/

/*!
    \fn const size_t *QCLWorkSize::sizes() const

    Returns a const pointer to the size array within this object.
*/

QT_END_NAMESPACE
