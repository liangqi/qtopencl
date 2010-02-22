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
#include "qclcontext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLSampler
    \brief The QCLSampler class represents an OpenCL sampler object.
    \since 4.7
    \ingroup opencl
*/

class QCLSamplerPrivate
{
public:
    QCLSamplerPrivate()
        : id(0)
        , context(0)
        , normalizedCoordinates(false)
        , addressingMode(QCLSampler::ClampToEdge)
        , filterMode(QCLSampler::Linear)
    {
    }
    QCLSamplerPrivate(const QCLSamplerPrivate *other)
        : id(other->id)
        , context(other->context)
        , normalizedCoordinates(other->normalizedCoordinates)
        , addressingMode(other->addressingMode)
        , filterMode(other->filterMode)
    {
        if (id)
            clRetainSampler(id);
    }

    void assign(const QCLSamplerPrivate *other)
    {
        if (id)
            clReleaseSampler(id);
        id = other->id;
        if (id)
            clRetainSampler(id);
        context = other->context;
        normalizedCoordinates = other->normalizedCoordinates;
        addressingMode = other->addressingMode;
        filterMode = other->filterMode;
    }

    void detach()
    {
        if (id) {
            clReleaseSampler(id);
            id = 0;
        }
    }

    mutable cl_sampler id;
    QCLContext *context;
    bool normalizedCoordinates;
    QCLSampler::AddressingMode addressingMode;
    QCLSampler::FilterMode filterMode;
};

/*!
    Constructs a default OpenCL sampler object, with unnormalized
    co-ordinates, clamp-to-edge addressing, and linear filtering.
*/
QCLSampler::QCLSampler()
    : d_ptr(new QCLSamplerPrivate())
{
}

/*!
    Constructs an OpenCL sampler object from the native identifier \a id.
    This class takes over ownership of \a id and will release it in
    the destructor.
*/
QCLSampler::QCLSampler(cl_sampler id)
    : d_ptr(new QCLSamplerPrivate())
{
    Q_D(QCLSampler);
    d->id = id;

    // Read the current settings from the OpenCL implementation.
    if (id) {
        cl_bool normalized = CL_FALSE;
        cl_addressing_mode addressing = CL_ADDRESS_CLAMP_TO_EDGE;
        cl_filter_mode filter = CL_FILTER_LINEAR;
        clGetSamplerInfo(id, CL_SAMPLER_NORMALIZED_COORDS,
                         sizeof(normalized), &normalized, 0);
        clGetSamplerInfo(id, CL_SAMPLER_ADDRESSING_MODE,
                         sizeof(addressing), &addressing, 0);
        clGetSamplerInfo(id, CL_SAMPLER_FILTER_MODE,
                         sizeof(filter), &filter, 0);
        d->normalizedCoordinates = (normalized != CL_FALSE);
        d->addressingMode = QCLSampler::AddressingMode(addressing);
        d->filterMode = QCLSampler::FilterMode(filter);
    }
}

/*!
    Constructs a copy of \a other.  The \c{clRetainSampler()} function
    will be called to update the reference count on samplerId().
*/
QCLSampler::QCLSampler(const QCLSampler &other)
    : d_ptr(new QCLSamplerPrivate(other.d_ptr.data()))
{
}

/*!
    Releases this OpenCL sampler object by calling \c{clReleaseSampler()}.
*/
QCLSampler::~QCLSampler()
{
    Q_D(QCLSampler);
    d->detach();
}

/*!
    Assigns \a other to this OpenCL sampler object.  The current samplerId()
    will be released with \c{clReleaseSampler()}, and the new samplerId()
    will be retained with \c{clRetainSampler()}.
*/
QCLSampler &QCLSampler::operator=(const QCLSampler &other)
{
    Q_D(QCLSampler);
    if (d != other.d_ptr.data())
        d->assign(other.d_ptr.data());
    return *this;
}

/*!
    \enum QCLSampler::AddressingMode
    This enum specifies how to handle out-of-range image co-ordinates
    when reading from an image in OpenCL.

    \value None No special handling of out-of-range co-ordinates.
    \value ClampToEdge Out-of-range requests clamp to the edge pixel value.
    \value Clamp Out-of-range requests clamp to the image extents.
    \value Repeat Repeats the image in a cycle.
*/

/*!
    \enum QCLSampler::FilterMode
    This enum defines the type of filter to apply when reading from
    an image in OpenCL.

    \value Nearest Use the color of the nearest pixel.
    \value Linear Interpolate linearly between pixel colors to generate
    intermediate pixel colors.
*/

/*!
    Returns true if this sampler is using normalized co-ordinates;
    false otherwise.  The default value is false.

    \sa setNormalizedCoordinates()
*/
bool QCLSampler::normalizedCoordinates() const
{
    Q_D(const QCLSampler);
    return d->normalizedCoordinates;
}

/*!
    Enables or disables normalized co-ordinates according to \a value.

    This function will release samplerId() if \a value is different
    than the previous value.

    \sa normalizedCoordinates()
*/
void QCLSampler::setNormalizedCoordinates(bool value)
{
    Q_D(QCLSampler);
    if (d->normalizedCoordinates != value) {
        d->detach();
        d->normalizedCoordinates = value;
    }
}

/*!
    Returns the addressing mode for out-of-range co-ordinates
    when reading from an image in OpenCL.  The default value is
    ClampToEdge.

    \sa setAddressingMode()
*/
QCLSampler::AddressingMode QCLSampler::addressingMode() const
{
    Q_D(const QCLSampler);
    return d->addressingMode;
}

/*!
    Sets the addressing mode for out-of-range co-ordinates
    when reading from an image in OpenCL to \a value.

    This function will release samplerId() if \a value is different
    than the previous value.

    \sa addressingMode()
*/
void QCLSampler::setAddressingMode(QCLSampler::AddressingMode value)
{
    Q_D(QCLSampler);
    if (d->addressingMode != value) {
        d->detach();
        d->addressingMode = value;
    }
}

/*!
    Returns the type of filter to apply when reading from an image
    in OpenCL.  The default value is Linear.

    \sa setFilterMode()
*/
QCLSampler::FilterMode QCLSampler::filterMode() const
{
    Q_D(const QCLSampler);
    return d->filterMode;
}

/*!
    Sets the type of filter to apply when reading from an image
    in OpenCL to \a value.

    This function will release samplerId() if \a value is different
    than the previous value.

    \sa filterMode()
*/
void QCLSampler::setFilterMode(QCLSampler::FilterMode value)
{
    Q_D(QCLSampler);
    if (d->filterMode != value) {
        d->detach();
        d->filterMode = value;
    }
}

/*!
    Returns the native OpenCL identifier for this sampler; or 0 if
    the identifier has not been created.

    The identifier is created when the sampler is set on a kernel
    as an argument with QCLKernel::setArg().
*/
cl_sampler QCLSampler::samplerId() const
{
    Q_D(const QCLSampler);
    return d->id;
}

/*!
    Returns the OpenCL context that this sampler was created for;
    null if not yet created within a context.
*/
QCLContext *QCLSampler::context() const
{
    Q_D(const QCLSampler);
    return d->context;
}

/*!
    Returns true if this OpenCL sampler has the same parameters
    as \a other; false otherwise.

    \sa operator!=()
*/
bool QCLSampler::operator==(const QCLSampler &other) const
{
    return d_ptr->normalizedCoordinates ==
                other.d_ptr->normalizedCoordinates &&
           d_ptr->addressingMode == other.d_ptr->addressingMode &&
           d_ptr->filterMode == other.d_ptr->filterMode;
}

/*!
    \fn bool QCLSampler::operator!=(const QCLSampler &other) const

    Returns true if this OpenCL sampler does not have the same
    parameters as \a other; false otherwise.

    \sa operator==()
*/

/*!
    \internal
*/
void QCLSampler::setKernelArg
    (QCLContext *context, cl_kernel kernel, int index) const
{
    Q_D(const QCLSampler);
    if (!d->id || d->context != context) {
        // Create a new sampler in the kernel's context.
        if (d->id)
            clReleaseSampler(d->id);    // Was created for another context.
        cl_int error;
        d->id = clCreateSampler
            (context->contextId(),
             d->normalizedCoordinates ? CL_TRUE : CL_FALSE,
             cl_addressing_mode(d->addressingMode),
             cl_filter_mode(d->filterMode), &error);
        context->reportError("QCLKernel::setArg(sampler):", error);
    }
    clSetKernelArg(kernel, index, sizeof(d->id), &(d->id));
}

QT_END_NAMESPACE
