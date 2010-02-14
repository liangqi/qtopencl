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

#include <stdio.h>
#include "qclplatform.h"
#include "qcldevice.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    printf("OpenCL Platforms:\n");
    QList<QCLPlatform> platforms = QCLPlatform::platforms();
    foreach (QCLPlatform platform, platforms) {
        printf("    Platform ID    : %ld\n", long(platform.id()));
        printf("    Profile        : %s\n", platform.profile().toLatin1().constData());
        printf("    Version        : %s\n", platform.version().toLatin1().constData());
        printf("    Name           : %s\n", platform.name().toLatin1().constData());
        printf("    Vendor         : %s\n", platform.vendor().toLatin1().constData());
        printf("    Extensions     :\n");
        QStringList extns = platform.extensions();
        foreach (QString ext, extns)
            printf("        %s\n", ext.toLatin1().constData());
        printf("\n");
    }

    printf("OpenCL Devices:\n");
    QList<QCLDevice> devices = QCLDevice::devices();
    foreach (QCLDevice dev, devices) {
        printf("    Device ID      : %ld\n", long(dev.id()));
        printf("    Platform ID    : %ld\n", long(dev.platform().id()));
        printf("    Vendor ID      : %u\n", dev.paramUInt(CL_DEVICE_VENDOR_ID));
        printf("    Type           :");
        QCLDevice::DeviceTypes type = dev.deviceType();
        if (type & QCLDevice::Default)
            printf(" Default");
        if (type & QCLDevice::CPU)
            printf(" CPU");
        if (type & QCLDevice::GPU)
            printf(" GPU");
        if (type & QCLDevice::Accelerator)
            printf(" Accelerator");
        printf("\n");
        printf("    Profile        : %s\n", dev.profile().toLatin1().constData());
        printf("    Version        : %s\n", dev.version().toLatin1().constData());
        printf("    Driver Version : %s\n", dev.driverVersion().toLatin1().constData());
        printf("    Name           : %s\n", dev.name().toLatin1().constData());
        printf("    Vendor         : %s\n", dev.vendor().toLatin1().constData());
        printf("    Compute Units  : %u\n", dev.paramUInt(CL_DEVICE_MAX_COMPUTE_UNITS));
        QCLWorkSize size = dev.maximumWorkItemSize();
        printf("    Max Work Size  : %ux%ux%u\n",
               uint(size.width()), uint(size.height()),
               uint(size.depth()));
        printf("    Max Items/Group: %u\n", uint(dev.maximumWorkItemsPerGroup()));
        printf("    Available      : %s\n",
               dev.isAvailable() ? "true" : "false");
        printf("    Image Support  : %s\n",
               dev.paramBool(CL_DEVICE_IMAGE_SUPPORT) ? "true" : "false");
        printf("    Has Compiler   : %s\n",
               dev.paramBool(CL_DEVICE_COMPILER_AVAILABLE) ? "true" : "false");
        printf("    Extensions     :\n");
        QStringList extns = dev.extensions();
        foreach (QString ext, extns)
            printf("        %s\n", ext.toLatin1().constData());
        printf("\n");
    }

    return 0;
}
