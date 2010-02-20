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

#ifndef QCLDEVICE_H
#define QCLDEVICE_H

#include "qclplatform.h"
#include "qclworksize.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class Q_CL_EXPORT QCLDevice
{
public:
    QCLDevice() : m_id(0) {}
    QCLDevice(cl_device_id id) : m_id(id) {}

    enum DeviceType
    {
        Default             = 0x00000001,
        CPU                 = 0x00000002,
        GPU                 = 0x00000004,
        Accelerator         = 0x00000008,
        All                 = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(DeviceTypes, DeviceType)

    bool isNull() const { return m_id == 0; }
    QCLDevice::DeviceTypes deviceType() const;
    QCLPlatform platform() const;
    bool isAvailable() const;

    QCLWorkSize maximumWorkItemSize() const;
    size_t maximumWorkItemsPerGroup() const;

    uint paramUInt(cl_device_info name, uint defaultValue = 0) const;
    quint64 paramULong(cl_device_info name, quint64 defaultValue = 0) const;
    size_t paramSize(cl_device_info name, size_t defaultValue = 0) const;
    bool paramBool(cl_device_info name, bool defaultValue = false) const;
    QString paramString(cl_device_info name) const;

    QString profile() const;
    QString version() const;
    QString driverVersion() const;
    QString name() const;
    QString vendor() const;
    QStringList extensions() const;

    cl_device_id id() const { return m_id; }

    static QList<QCLDevice> devices(QCLDevice::DeviceTypes types = All);
    static QList<QCLDevice> devices
        (QCLDevice::DeviceTypes types, const QCLPlatform &platform);
    static QList<QCLDevice> devices
        (QCLDevice::DeviceTypes types, const QList<QCLPlatform> &platforms);

    bool operator==(const QCLDevice &other) const;
    bool operator!=(const QCLDevice &other) const;

private:
    cl_device_id m_id;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QCLDevice::DeviceTypes)

inline bool QCLDevice::operator==(const QCLDevice &other) const
{
    return m_id == other.m_id;
}

inline bool QCLDevice::operator!=(const QCLDevice &other) const
{
    return m_id != other.m_id;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
