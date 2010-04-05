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

#ifndef QCLSAMPLER_H
#define QCLSAMPLER_H

#include "qclglobal.h"
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLSamplerPrivate;
class QCLContext;

class Q_CL_EXPORT QCLSampler
{
public:
    QCLSampler();
    QCLSampler(QCLContext *context, cl_sampler id);
    QCLSampler(const QCLSampler &other);
    ~QCLSampler();

    QCLSampler &operator=(const QCLSampler &other);

    enum AddressingMode
    {
        None                = 0x1130,   // CL_ADDRESS_NONE
        ClampToEdge         = 0x1131,   // CL_ADDRESS_CLAMP_TO_EDGE
        Clamp               = 0x1132,   // CL_ADDRESS_CLAMP
        Repeat              = 0x1133    // CL_ADDRESS_REPEAT
    };

    enum FilterMode
    {
        Nearest             = 0x1140,   // CL_FILTER_NEAREST
        Linear              = 0x1141    // CL_FILTER_LINEAR
    };

    bool normalizedCoordinates() const;
    void setNormalizedCoordinates(bool value);

    QCLSampler::AddressingMode addressingMode() const;
    void setAddressingMode(QCLSampler::AddressingMode value);

    QCLSampler::FilterMode filterMode() const;
    void setFilterMode(QCLSampler::FilterMode value);

    cl_sampler samplerId() const;
    QCLContext *context() const;

    bool operator==(const QCLSampler &other) const;
    bool operator!=(const QCLSampler &other) const;

private:
    QScopedPointer<QCLSamplerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QCLSampler)

    void setKernelArg(QCLContext *context, cl_kernel kernel, int index) const;

    friend class QCLContext;
    friend class QCLKernel;
};

inline bool QCLSampler::operator!=(const QCLSampler &other) const
{
    return !(*this == other);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
