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

static uint qt_cl_paramUInt(cl_device_id id, cl_device_info name)
{
    cl_uint value;
    if (clGetDeviceInfo(id, name, sizeof(value), &value, 0)
            != CL_SUCCESS)
        return 0;
    else
        return uint(value);
}

static int qt_cl_paramInt(cl_device_id id, cl_device_info name)
{
    cl_int value;
    if (clGetDeviceInfo(id, name, sizeof(value), &value, 0)
            != CL_SUCCESS)
        return 0;
    else
        return int(value);
}

static quint64 qt_cl_paramULong(cl_device_id id, cl_device_info name)
{
    cl_ulong value;
    if (clGetDeviceInfo(id, name, sizeof(value), &value, 0)
            != CL_SUCCESS)
        return 0;
    else
        return quint64(value);
}

static size_t qt_cl_paramSize(cl_device_id id, cl_device_info name)
{
    size_t value;
    if (clGetDeviceInfo(id, name, sizeof(value), &value, 0)
            != CL_SUCCESS)
        return 0;
    else
        return value;
}

static bool qt_cl_paramBool(cl_device_id id, cl_device_info name)
{
    cl_bool value;
    if (clGetDeviceInfo(id, name, sizeof(value), &value, 0)
            != CL_SUCCESS)
        return false;
    else
        return value != 0;
}

static QString qt_cl_paramString(cl_device_id id, cl_device_info name)
{
    size_t size;
    if (clGetDeviceInfo(id, name, 0, 0, &size) != CL_SUCCESS)
        return QString();
    QVarLengthArray<char> buf(size);
    if (clGetDeviceInfo(id, name, size, buf.data(), &size) != CL_SUCCESS)
        return QString();
    return QString::fromLatin1(buf.data());
}

/*!
    Returns the type of this device.  It is possible for a device
    to have more than one type.
*/
QCLDevice::DeviceTypes QCLDevice::deviceType() const
{
    cl_device_type type;
    if (clGetDeviceInfo(m_id, CL_DEVICE_TYPE, sizeof(type), &type, 0)
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
    if (clGetDeviceInfo(m_id, CL_DEVICE_PLATFORM, sizeof(plat), &plat, 0)
            != CL_SUCCESS)
        return QCLPlatform();
    else
        return QCLPlatform(plat);
}

/*!
    Returns the vendor's identifier for this device.
*/
uint QCLDevice::vendorId() const
{
    return qt_cl_paramUInt(m_id, CL_DEVICE_VENDOR_ID);
}

/*!
    Returns true if this device is available; false otherwise.
*/
bool QCLDevice::isAvailable() const
{
    return qt_cl_paramBool(m_id, CL_DEVICE_AVAILABLE);
}

/*!
    Returns true if this device has a compiler available;
    false otherwise.
*/
bool QCLDevice::hasCompiler() const
{
    return qt_cl_paramBool(m_id, CL_DEVICE_COMPILER_AVAILABLE);
}

/*!
    Returns true if this device has support for executing
    native kernels; false otherwise.
*/
bool QCLDevice::hasNativeKernels() const
{
    cl_device_exec_capabilities caps;
    if (clGetDeviceInfo(m_id, CL_DEVICE_EXECUTION_CAPABILITIES,
                        sizeof(caps), &caps, 0)
            != CL_SUCCESS)
        return false;
    else
        return (caps & CL_EXEC_NATIVE_KERNEL) != 0;
}

/*!
    Returns true if this device supports out of order execution
    of commands on a QCLCommandQueue; false otherwise.

    \sa QCLCommandQueue::isOutOfOrder()
*/
bool QCLDevice::hasOutOfOrderExecution() const
{
    cl_command_queue_properties props;
    if (clGetDeviceInfo(m_id, CL_DEVICE_QUEUE_PROPERTIES,
                        sizeof(props), &props, 0)
            != CL_SUCCESS)
        return false;
    else
        return (props & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) != 0;
}

/*!
    Returns true if this device supports the \c{double} type
    via the \c{cl_khr_fp64} extension; false otherwise.
*/
bool QCLDevice::hasDouble() const
{
    return extensions().contains
        (QLatin1String("cl_khr_fp64"), Qt::CaseInsensitive);
}

/*!
    Returns true if this device supports operations on the
    \c{half} type via the \c{cl_khr_fp16} extension;
    false otherwise.

    Note: \c{half} is supported by the OpenCL 1.0 core specification
    for data storage even if this function returns false.
    However, kernels can only perform arithmetic operations on
    \c{half} values if this function returns true.
*/
bool QCLDevice::hasHalfFloat() const
{
    return extensions().contains
        (QLatin1String("cl_khr_fp16"), Qt::CaseInsensitive);
}

/*!
    Returns true if the device implements error correction on
    its memory areas; false otherwise.
*/
bool QCLDevice::hasErrorCorrectingMemory() const
{
    return qt_cl_paramBool(m_id, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
}

/*!
    Returns the number of parallel compute units on the device.
*/
int QCLDevice::computeUnits() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_COMPUTE_UNITS);
}

/*!
    Returns the maximum clock frequency for this device in MHz.
*/
int QCLDevice::clockFrequency() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_CLOCK_FREQUENCY);
}

/*!
    Returns the number of address bits used by the device;
    usually 32 or 64.
*/
int QCLDevice::addressBits() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_ADDRESS_BITS);
}

/*!
    Returns the byte order of the device, indicating little
    endian or big endian.
*/
QSysInfo::Endian QCLDevice::byteOrder() const
{
    if (qt_cl_paramBool(m_id, CL_DEVICE_ENDIAN_LITTLE))
        return QSysInfo::LittleEndian;
    else
        return QSysInfo::BigEndian;
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
int QCLDevice::maximumWorkItemsPerGroup() const
{
    return int(qt_cl_paramSize(m_id, CL_DEVICE_MAX_WORK_GROUP_SIZE));
}

/*!
    Returns true if this device has image support; false otherwise.
*/
bool QCLDevice::hasImages() const
{
    return qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT);
}

/*!
    Returns true if this device supports writing to 3D images
    via the \c{cl_khr_3d_image_writes} extension; false otherwise.
*/
bool QCLDevice::hasWritable3DImages() const
{
    return extensions().contains
        (QLatin1String("cl_khr_3d_image_writes"), Qt::CaseInsensitive);
}

/*!
    Returns the maximum size of 2D images that are supported
    by this device; or an empty QSize if images are not supported.

    \sa maximumImage3DSize(), hasImages()
*/
QSize QCLDevice::maximumImage2DSize() const
{
    if (!qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT))
        return QSize();
    return QSize(qt_cl_paramInt(m_id, CL_DEVICE_IMAGE2D_MAX_WIDTH),
                 qt_cl_paramInt(m_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT));
}

/*!
    Returns the maximum size of 3D images that are supported
    by this device; or a (0, 0, 0) if images are not supported.

    \sa maximumImage2DSize(), hasImages()
*/
QCLWorkSize QCLDevice::maximumImage3DSize() const
{
    if (!qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT))
        return QCLWorkSize(0, 0, 0);
    return QCLWorkSize(qt_cl_paramSize(m_id, CL_DEVICE_IMAGE3D_MAX_WIDTH),
                       qt_cl_paramSize(m_id, CL_DEVICE_IMAGE3D_MAX_HEIGHT),
                       qt_cl_paramSize(m_id, CL_DEVICE_IMAGE3D_MAX_DEPTH));
}

/*!
    Returns the maximum number of image samplers that can be used
    in a kernel at one time; 0 if images are not supported.

    \sa hasImages()
*/
int QCLDevice::maximumSamplers() const
{
    if (!qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT))
        return 0;
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_SAMPLERS);
}

/*!
    Returns the maximum number of image objects that can be
    read simultaneously by a kernel; 0 if images are not supported.

    \sa maximumWriteImages(), hasImages()
*/
int QCLDevice::maximumReadImages() const
{
    if (!qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT))
        return 0;
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_READ_IMAGE_ARGS);
}

/*!
    Returns the maximum number of image objects that can be
    written simultaneously by a kernel; 0 if images are not supported.

    \sa maximumReadImages(), hasImages()
*/
int QCLDevice::maximumWriteImages() const
{
    if (!qt_cl_paramBool(m_id, CL_DEVICE_IMAGE_SUPPORT))
        return 0;
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
}

/*!
    Returns the preferred size for vectors of type \c{char}
    in the device.  For example, 4 indicates that 4 \c{char}
    values can be packed into a vector and operated on as a
    unit for optimal performance.
*/
int QCLDevice::preferredCharVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
}

/*!
    Returns the preferred size for vectors of type \c{short}
    in the device.  For example, 4 indicates that 4 \c{short}
    values can be packed into a vector and operated on as a
    unit for optimal performance.
*/
int QCLDevice::preferredShortVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
}

/*!
    Returns the preferred size for vectors of type \c{int}
    in the device.  For example, 4 indicates that 4 \c{int}
    values can be packed into a vector and operated on as a
    unit for optimal performance.
*/
int QCLDevice::preferredIntVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
}

/*!
    Returns the preferred size for vectors of type \c{long}
    in the device.  For example, 2 indicates that 2 \c{long}
    values can be packed into a vector and operated on as a
    unit for optimal performance.
*/
int QCLDevice::preferredLongVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
}

/*!
    Returns the preferred size for vectors of type \c{float}
    in the device.  For example, 4 indicates that 4 \c{float}
    values can be packed into a vector and operated on as a
    unit for optimal performance.
*/
int QCLDevice::preferredFloatVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
}

/*!
    Returns the preferred size for vectors of type \c{double}
    in the device.  For example, 2 indicates that 2 \c{double}
    values can be packed into a vector and operated on as a
    unit for optimal performance.

    Returns zero if the device does not support \c{double}.

    \sa hasDouble()
*/
int QCLDevice::preferredDoubleVectorSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
}

/*!
    \enum QCLDevice::FloatCapability
    This enum defines the floating-point capabilities of the
    \c{float} or \c{double} type on an OpenCL device.

    \value NotSupported Returned to indicate that \c{double} is
           not supported on the device.
    \value Denorm Denorms are supported.
    \value InfinityNaN Infinity and quiet NaN's are supported.
    \value RoundNearest Round to nearest even rounding mode supported.
    \value RoundZero Round to zero rounding mode supported.
    \value RoundInfinity Round to infinity rounding mode supported.
    \value FusedMultiplyAdd IEEE754-2008 fused multiply-add
           is supported.
*/

/*!
    Returns a set of flags that describes the floating-point
    capabilities of the \c{float} type on this device.
*/
QCLDevice::FloatCapabilities QCLDevice::floatCapabilities() const
{
    cl_device_fp_config config;
    if (clGetDeviceInfo(m_id, CL_DEVICE_SINGLE_FP_CONFIG,
                        sizeof(config), &config, 0)
            != CL_SUCCESS)
        return NotSupported;
    else
        return QCLDevice::FloatCapabilities(config);
}

#ifndef CL_DEVICE_DOUBLE_FP_CONFIG
#define CL_DEVICE_DOUBLE_FP_CONFIG 0x1032
#endif

/*!
    Returns a set of flags that describes the floating-point
    capabilities of the \c{double} type on this device.

    Returns QCLDevice::NotSupported if operations on \c{double}
    are not supported by the device.

    \sa hasDouble()
*/
QCLDevice::FloatCapabilities QCLDevice::doubleCapabilities() const
{
    cl_device_fp_config config;
    if (clGetDeviceInfo(m_id, CL_DEVICE_DOUBLE_FP_CONFIG,
                        sizeof(config), &config, 0)
            != CL_SUCCESS)
        return NotSupported;
    else
        return QCLDevice::FloatCapabilities(config);
}

#ifndef CL_DEVICE_HALF_FP_CONFIG
#define CL_DEVICE_HALF_FP_CONFIG 0x1033
#endif

/*!
    Returns a set of flags that describes the floating-point
    capabilities of the \c{half} type on this device.

    Returns QCLDevice::NotSupported if operations on \c{half}
    are not supported by the device.

    \sa hasHalfFloat()
*/
QCLDevice::FloatCapabilities QCLDevice::halfFloatCapabilities() const
{
    cl_device_fp_config config;
    if (clGetDeviceInfo(m_id, CL_DEVICE_HALF_FP_CONFIG,
                        sizeof(config), &config, 0)
            != CL_SUCCESS)
        return NotSupported;
    else
        return QCLDevice::FloatCapabilities(config);
}

/*!
    Returns the resolution of the device profiling timer in
    nanoseconds.

    \sa QCLEvent::finishTime(), QCLCommandQueue::setProfilingEnabled()
*/
quint64 QCLDevice::profilingTimerResolution() const
{
    // Spec says size_t, even though actual times are cl_ulong.
    return qt_cl_paramSize(m_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
}

/*!
    Returns the maximum memory allocation size in bytes.

    \sa globalMemorySize()
*/
quint64 QCLDevice::maximumAllocationSize() const
{
    return qt_cl_paramULong(m_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
}

/*!
    Returns the number of bytes of global memory in the device.

    \sa globalMemoryCacheSize(), localMemorySize()
*/
quint64 QCLDevice::globalMemorySize() const
{
    return qt_cl_paramULong(m_id, CL_DEVICE_GLOBAL_MEM_SIZE);
}

/*!
    \enum QCLDevice::CacheType
    This enum defines the type of global memory cache that is
    supported by an OpenCL device.

    \value NoCache No global memory cache.
    \value ReadOnlyCache Read-only global memory cache.
    \value ReadWriteCache Read-write global memory cache.
*/

/*!
    Returns the type of global memory cache that is supported
    by the device.
*/
QCLDevice::CacheType QCLDevice::globalMemoryCacheType() const
{
    cl_device_mem_cache_type type;
    if (clGetDeviceInfo(m_id, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
                        sizeof(type), &type, 0)
            != CL_SUCCESS)
        return NoCache;
    else
        return QCLDevice::CacheType(type);
}

/*!
    Returns the size of the global memory cache in bytes.

    \sa globalMemorySize(), globalMemoryCacheLineSize()
*/
quint64 QCLDevice::globalMemoryCacheSize() const
{
    return qt_cl_paramULong(m_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
}

/*!
    Returns the size of a single global memory cache line in bytes.

    \sa globalMemoryCacheSize()
*/
int QCLDevice::globalMemoryCacheLineSize() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
}

/*!
    Returns the number of bytes of local memory in the device.

    \sa globalMemorySize(), isLocalMemorySeparate()
*/
quint64 QCLDevice::localMemorySize() const
{
    return qt_cl_paramULong(m_id, CL_DEVICE_LOCAL_MEM_SIZE);
}

/*!
    Returns true if the local memory on this device is in a separate
    dedicated storage area from global memory; false if local memory
    is allocated from global memory.

    \sa localMemorySize()
*/
bool QCLDevice::isLocalMemorySeparate() const
{
    cl_device_local_mem_type type;
    if (clGetDeviceInfo(m_id, CL_DEVICE_LOCAL_MEM_TYPE,
                        sizeof(type), &type, 0)
            != CL_SUCCESS)
        return false;
    else
        return type == CL_LOCAL;
}

/*!
    Returns the maximum size for a constant buffer allocation.

    \sa maximumConstantArguments()
*/
quint64 QCLDevice::maximumConstantBufferSize() const
{
    return qt_cl_paramULong(m_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
}

/*!
    Returns the maximum number of constant arguments that can
    be passed to a kernel.

    \sa maximumConstantBufferSize()
*/
int QCLDevice::maximumConstantArguments() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_MAX_CONSTANT_ARGS);
}

/*!
    Returns the default alignment for allocated memory in bytes.

    \sa minimumAlignment()
*/
int QCLDevice::defaultAlignment() const
{
    // OpenCL setting is in bits, but that is inconsistent with
    // every other alignment value, so return bytes instead.
    return qt_cl_paramInt(m_id, CL_DEVICE_MEM_BASE_ADDR_ALIGN) / 8;
}

/*!
    Returns the minimum alignment for any data type in bytes.

    \sa defaultAlignment()
*/
int QCLDevice::minimumAlignment() const
{
    return qt_cl_paramInt(m_id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
}

/*!
    Returns the maximum number of parameter bytes that can be passed
    to a kernel.
*/
int QCLDevice::maximumParameterBytes() const
{
    return int(qt_cl_paramSize(m_id, CL_DEVICE_MAX_PARAMETER_SIZE));
}

/*!
    Returns the profile that is implemented by this OpenCL device,
    usually \c FULL_PROFILE or \c EMBEDDED_PROFILE.
*/
QString QCLDevice::profile() const
{
    return qt_cl_paramString(m_id, CL_DEVICE_PROFILE);
}

/*!
    Returns the OpenCL version that is implemented by this OpenCL device,
    usually something like \c{OpenCL 1.0}.

    \sa driverVersion()
*/
QString QCLDevice::version() const
{
    return qt_cl_paramString(m_id, CL_DEVICE_VERSION);
}

/*!
    Returns the driver version of this OpenCL device.

    \sa version()
*/
QString QCLDevice::driverVersion() const
{
    return qt_cl_paramString(m_id, CL_DRIVER_VERSION);
}

/*!
    Returns the name of this OpenCL device.
*/
QString QCLDevice::name() const
{
    return qt_cl_paramString(m_id, CL_DEVICE_NAME);
}

/*!
    Returns the vendor of this OpenCL device.
*/
QString QCLDevice::vendor() const
{
    return qt_cl_paramString(m_id, CL_DEVICE_VENDOR);
}

/*!
    Returns a list of the extensions supported by this OpenCL device.
*/
QStringList QCLDevice::extensions() const
{
    return qt_cl_paramString(m_id, CL_DEVICE_EXTENSIONS).simplified().split(QChar(' '));
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

/*!
    \fn bool QCLDevice::operator==(const QCLDevice &other) const

    Returns true if this OpenCL device identifier is the same
    as \a other; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QCLDevice::operator!=(const QCLDevice &other) const

    Returns true if this OpenCL device identifier is not the
    same as \a other; false otherwise.

    \sa operator==()
*/

QT_END_NAMESPACE
