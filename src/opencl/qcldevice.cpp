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

#include "qcldevice.h"
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLDevice
    \brief The QCLDevice class represents an OpenCL device definition.
    \since 4.7
    \ingroup opencl
*/

/*!
    \fn QCLDevice::QCLDevice()

    Constructs a default OpenCL device identifier.
*/

/*!
    \fn QCLDevice::QCLDevice(cl_device_id id)

    Constructs an OpenCL device identifier that corresponds to the
    native OpenCL value \a id.
*/

/*!
    \fn bool QCLDevice::isNull() const

    Returns true if this OpenCL device identifier is null.
*/

/*!
    \enum QCLDevice::DeviceType
    This enum defines the type of OpenCL device that is represented
    by a QCLDevice object.

    \value Default The default OpenCL device.
    \value CPU The host CPU within the OpenCL system.
    \value GPU An OpenCL device that is also an OpenGL GPU.
    \value Accelerator Dedicated OpenCL accelerator.
    \value All All OpenCL device types.
*/

/*!
    Returns the type of this device.  It is possible for a device
    to have more than one type.
*/
QCLDevice::DeviceTypes QCLDevice::deviceType() const
{
    cl_device_type type;
    size_t size;
    if (clGetDeviceInfo(m_id, CL_DEVICE_TYPE, sizeof(type), &type, &size)
            != CL_SUCCESS)
        return QCLDevice::DeviceTypes(0);
    else
        return QCLDevice::DeviceTypes(type);
}

/*!
    Returns the platform identifier for this device.
*/
QCLPlatform QCLDevice::platform() const
{
    cl_platform_id plat;
    size_t size;
    if (clGetDeviceInfo(m_id, CL_DEVICE_PLATFORM, sizeof(plat), &plat, &size)
            != CL_SUCCESS)
        return QCLPlatform();
    else
        return QCLPlatform(plat);
}

/*!
    Returns true if this device is available; false otherwise.
*/
bool QCLDevice::isAvailable() const
{
    return paramBool(CL_DEVICE_AVAILABLE);
}

/*!
    Returns the maximum work size for this device.

    \sa maximumWorkItemsPerGroup()
*/
QCLWorkSize QCLDevice::maximumWorkItemSize() const
{
    size_t dims = 0;
    if (clGetDeviceInfo(m_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                        sizeof(dims), &dims, 0) != CL_SUCCESS || !dims)
        return QCLWorkSize(1, 1, 1);
    QVarLengthArray<size_t> buf(dims);
    if (clGetDeviceInfo(m_id, CL_DEVICE_MAX_WORK_ITEM_SIZES,
                        sizeof(size_t) * dims, buf.data(), 0) != CL_SUCCESS)
        return QCLWorkSize(1, 1, 1);
    if (dims == 1)
        return QCLWorkSize(buf[0]);
    else if (dims == 2)
        return QCLWorkSize(buf[0], buf[1]);
    else
        return QCLWorkSize(buf[0], buf[1], buf[2]);
}

/*!
    Returns the maximum number of work items in a work group executing a
    kernel using data parallel execution.

    \sa maximumWorkItemSize()
*/
size_t QCLDevice::maximumWorkItemsPerGroup() const
{
    return paramSize(CL_DEVICE_MAX_WORK_GROUP_SIZE);
}

/*!
    Returns the unsigned integer parameter \a name on this device.
    If the parameter is not present, \a defaultValue will be returned.
*/
uint QCLDevice::paramUInt(cl_device_info name, uint defaultValue) const
{
    cl_uint value;
    size_t size;
    if (clGetDeviceInfo(m_id, name, sizeof(value), &value, &size)
            != CL_SUCCESS)
        return defaultValue;
    else
        return uint(value);
}

/*!
    Returns the unsigned long parameter \a name on this device.
    If the parameter is not present, \a defaultValue will be returned.
*/
quint64 QCLDevice::paramULong(cl_device_info name, quint64 defaultValue) const
{
    cl_ulong value;
    size_t size;
    if (clGetDeviceInfo(m_id, name, sizeof(value), &value, &size)
            != CL_SUCCESS)
        return defaultValue;
    else
        return quint64(value);
}

/*!
    Returns the size parameter \a name on this device.  If the parameter is
    not present, \a defaultValue will be returned.
*/
size_t QCLDevice::paramSize(cl_device_info name, size_t defaultValue) const
{
    size_t value;
    size_t size;
    if (clGetDeviceInfo(m_id, name, sizeof(value), &value, &size)
            != CL_SUCCESS)
        return defaultValue;
    else
        return value;
}

/*!
    Returns the boolean parameter \a name on this device.  If the parameter
    is not present, \a defaultValue will be returned.
*/
bool QCLDevice::paramBool(cl_device_info name, bool defaultValue) const
{
    cl_bool value;
    size_t size;
    if (clGetDeviceInfo(m_id, name, sizeof(value), &value, &size)
            != CL_SUCCESS)
        return defaultValue;
    else
        return value != 0;
}

/*!
    Returns the string parameter \a name on this device.
*/
QString QCLDevice::paramString(cl_device_info name) const
{
    size_t size;
    if (clGetDeviceInfo(m_id, name, 0, 0, &size) != CL_SUCCESS)
        return QString();
    QVarLengthArray<char> buf(size);
    if (clGetDeviceInfo(m_id, name, size, buf.data(), &size) != CL_SUCCESS)
        return QString();
    return QString::fromLatin1(buf.data());
}

/*!
    Returns the profile that is implemented by this OpenCL device,
    usually \c FULL_PROFILE or \c EMBEDDED_PROFILE.
*/
QString QCLDevice::profile() const
{
    return paramString(CL_DEVICE_PROFILE);
}

/*!
    Returns the OpenCL version that is implemented by this OpenCL device,
    usually something like \c{OpenCL 1.0}.

    \sa driverVersion()
*/
QString QCLDevice::version() const
{
    return paramString(CL_DEVICE_VERSION);
}

/*!
    Returns the driver version of this OpenCL device.

    \sa version()
*/
QString QCLDevice::driverVersion() const
{
    return paramString(CL_DRIVER_VERSION);
}

/*!
    Returns the name of this OpenCL device.
*/
QString QCLDevice::name() const
{
    return paramString(CL_DEVICE_NAME);
}

/*!
    Returns the vendor of this OpenCL device.
*/
QString QCLDevice::vendor() const
{
    return paramString(CL_DEVICE_VENDOR);
}

/*!
    Returns a list of the extensions supported by this OpenCL device.
*/
QStringList QCLDevice::extensions() const
{
    return paramString(CL_DEVICE_EXTENSIONS).simplified().split(QChar(' '));
}

/*!
    \fn cl_device_id QCLDevice::id() const

    Returns the native OpenCL device identifier for this object.
*/

/*!
    \overload
    Returns a list of all OpenCL devices that match \a types on
    all platforms on this system.
*/
QList<QCLDevice> QCLDevice::devices(QCLDevice::DeviceTypes types)
{
    return devices(types, QCLPlatform::platforms());
}

/*!
    \overload
    Returns a list of all OpenCL devices that match \a types on
    \a platform on this system.
*/
QList<QCLDevice> QCLDevice::devices
    (QCLDevice::DeviceTypes types, const QCLPlatform &platform)
{
    QList<QCLPlatform> platforms;
    platforms.append(platform);
    return devices(types, platforms);
}

/*!
    Returns a list of all OpenCL devices that match \a types on
    the specified \a platforms list.
*/
QList<QCLDevice> QCLDevice::devices
    (QCLDevice::DeviceTypes types, const QList<QCLPlatform> &platforms)
{
    QList<QCLDevice> devs;
    for (int plat = 0; plat < platforms.size(); ++plat) {
        cl_uint size;
        if (clGetDeviceIDs(platforms[plat].id(), cl_device_type(types),
                           0, 0, &size) != CL_SUCCESS)
            continue;
        QVarLengthArray<cl_device_id> buf(size);
        if (clGetDeviceIDs(platforms[plat].id(), cl_device_type(types),
                           size, buf.data(), &size) != CL_SUCCESS)
            continue;
        for (int index = 0; index < buf.size(); ++index)
            devs.append(QCLDevice(buf[index]));
    }
    return devs;
}

QT_END_NAMESPACE
