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

#include "qclsampler.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLSampler
    \brief The QCLSampler class represents an OpenCL sampler object.
    \since 4.7
    \ingroup opencl
*/

/*!
    \fn QCLSampler::QCLSampler()

    Constructs a null OpenCL sampler object.
*/

/*!
    \fn QCLSampler::QCLSampler(cl_sampler id)

    Constructs an OpenCL sampler object from the native identifier \a id.
    This class takes over ownership of \a id and will release it in
    the destructor.
*/

/*!
    Constructs a copy of \a other.  The \c{clRetainSampler()} function
    will be called to update the reference count on id().
*/
QCLSampler::QCLSampler(const QCLSampler &other)
    : m_id(other.m_id)
{
    if (m_id)
        clRetainSampler(m_id);
}

/*!
    Releases this OpenCL sampler object by calling \c{clReleaseSampler()}.
*/
QCLSampler::~QCLSampler()
{
    if (m_id)
        clReleaseSampler(m_id);
}

/*!
    Assigns \a other to this OpenCL sampler object.  The current id() will
    be released with \c{clReleaseSampler()}, and the new id() will be
    retained with \c{clRetainSampler()}.
*/
QCLSampler &QCLSampler::operator=(const QCLSampler &other)
{
    if (m_id != other.m_id) {
        if (m_id)
            clReleaseSampler(m_id);
        m_id = other.m_id;
        if (m_id)
            clRetainSampler(m_id);
    }
    return *this;
}

/*!
    \fn bool QCLSampler::isNull() const

    Returns true if this OpenCL sampler object is null; false otherwise.
*/

/*!
    \fn cl_sampler QCLSampler::id() const

    Returns the native OpenCL identifier for this sampler.
*/

QT_END_NAMESPACE
