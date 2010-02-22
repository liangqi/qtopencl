/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include "qclcontext.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qpoint.h>

class tst_QCL : public QObject
{
    Q_OBJECT
public:
    tst_QCL() {}
    virtual ~tst_QCL() {}

private slots:
    void initTestCase();
    void checkProfile();
    void buildProgram();
    void argumentPassing();
    void vectorBuffer();
    void eventProfiling();

private:
    QCLContext context;
    QCLProgram program;
};

void tst_QCL::initTestCase()
{
    QVERIFY(context.create());
}

// Checks that the OpenCL implementation supports either
// FULL_PROFILE or EMBEDDED_PROFILE and that the device and
// platform are consistent.
void tst_QCL::checkProfile()
{
    QCLDevice device = context.defaultDevice();
    QCLPlatform platform = device.platform();
    QVERIFY((device.isFullProfile() && !device.isEmbeddedProfile()) ||
            (!device.isFullProfile() && device.isEmbeddedProfile()));
    QVERIFY((platform.isFullProfile() && !platform.isEmbeddedProfile()) ||
            (!platform.isFullProfile() && platform.isEmbeddedProfile()));
    QVERIFY(device.isFullProfile() == platform.isFullProfile());
    QVERIFY(device.isEmbeddedProfile() == platform.isEmbeddedProfile());
}

// Build the OpenCL program code we will be using for the rest of the tests.
void tst_QCL::buildProgram()
{
    program = context.buildProgramFromSourceFile(QLatin1String(":/tst_qcl.cl"));
    QVERIFY(!program.isNull());
}

// Tests that passing various argument types like int, float, QVector3D,
// QPointF, QMatrix4x4, etc to an OpenCL kernel does the right thing.
void tst_QCL::argumentPassing()
{
    QCLBuffer buffer = context.createBufferDevice
        (sizeof(float) * 16, QCL::WriteOnly);
    float buf[16];
    cl_int bufi[2];
    cl_uint bufui;
    cl_long buflong;
    cl_ulong bufulong;

    QCLKernel storeFloat = program.createKernel("storeFloat");
    storeFloat(buffer, 5.0f);
    buffer.read(buf, sizeof(float));
    QCOMPARE(buf[0], 5.0f);

    QCLKernel storeInt = program.createKernel("storeInt");
    storeInt(buffer, 7);
    buffer.read(bufi, sizeof(cl_int));
    QCOMPARE(bufi[0], cl_int(7));

    QCLKernel storeUInt = program.createKernel("storeUInt");
    storeInt(buffer, cl_uint(9));
    buffer.read(&bufui, sizeof(cl_uint));
    QCOMPARE(bufui, cl_uint(9));

    QCLKernel storeLong = program.createKernel("storeLong");
    storeLong(buffer, cl_long(-9));
    buffer.read(&buflong, sizeof(cl_long));
    QCOMPARE(buflong, cl_long(-9));

    QCLKernel storeULong = program.createKernel("storeULong");
    storeULong(buffer, cl_ulong(9));
    buffer.read(&bufulong, sizeof(cl_ulong));
    QCOMPARE(bufulong, cl_ulong(9));

    QCLKernel storeVec2 = program.createKernel("storeVec2");
    storeVec2(buffer, QVector2D(1.0f, 2.0f));
    buffer.read(buf, sizeof(float) * 2);
    QCOMPARE(buf[0], 1.0f);
    QCOMPARE(buf[1], 2.0f);
    storeVec2(buffer, QPointF(3.0f, 4.0f));
    buffer.read(buf, sizeof(float) * 2);
    QCOMPARE(buf[0], 3.0f);
    QCOMPARE(buf[1], 4.0f);

    QCLKernel storeVec2i = program.createKernel("storeVec2i");
    storeVec2(buffer, QPoint(6, -7));
    buffer.read(bufi, sizeof(cl_int) * 2);
    QCOMPARE(bufi[0], cl_int(6));
    QCOMPARE(bufi[1], cl_int(-7));

    QCLKernel storeVec3 = program.createKernel("storeVec3");
    storeVec3(buffer, QVector3D(-1.0f, -2.0f, -3.0f));
    buffer.read(buf, sizeof(float) * 4);
    QCOMPARE(buf[0], -1.0f);
    QCOMPARE(buf[1], -2.0f);
    QCOMPARE(buf[2], -3.0f);
    QCOMPARE(buf[3], 1.0f);

    QCLKernel storeVec4 = program.createKernel("storeVec4");
    storeVec4(buffer, QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    buffer.read(buf, sizeof(float) * 4);
    QCOMPARE(buf[0], 1.0f);
    QCOMPARE(buf[1], 2.0f);
    QCOMPARE(buf[2], 3.0f);
    QCOMPARE(buf[3], 4.0f);

    QMatrix4x4 m(1.0f, 2.0f, 3.0f, 4.0f,    // row-major input
                 5.0f, 6.0f, 7.0f, 8.0f,
                 9.0f, 10.0f, 11.0f, 12.0f,
                 13.0f, 14.0f, 15.0f, 16.0f);
    QCLKernel storeMat4 = program.createKernel("storeMat4");
    storeMat4(buffer, m);
    buffer.read(buf, sizeof(float) * 16);
    QCOMPARE(buf[0], 1.0f);                 // column-major output
    QCOMPARE(buf[1], 5.0f);
    QCOMPARE(buf[2], 9.0f);
    QCOMPARE(buf[3], 13.0f);
    QCOMPARE(buf[4], 2.0f);
    QCOMPARE(buf[5], 6.0f);
    QCOMPARE(buf[6], 10.0f);
    QCOMPARE(buf[7], 14.0f);
    QCOMPARE(buf[8], 3.0f);
    QCOMPARE(buf[9], 7.0f);
    QCOMPARE(buf[10], 11.0f);
    QCOMPARE(buf[11], 15.0f);
    QCOMPARE(buf[12], 4.0f);
    QCOMPARE(buf[13], 8.0f);
    QCOMPARE(buf[14], 12.0f);
    QCOMPARE(buf[15], 16.0f);
}

static float constVectorAt(const QCLVector<float>& vector, int index)
{
    return vector[index];
}

// Test the QCLVector<T> class.
void tst_QCL::vectorBuffer()
{
    QCLVector<float> vector1;
    QVERIFY(vector1.isNull());
    QVERIFY(vector1.isEmpty());
    QCOMPARE(vector1.size(), 0);
    QVERIFY(vector1.memoryId() == 0);
    QVERIFY(vector1.context() == 0);
    QVERIFY(!vector1.isMapped());

    vector1 = context.createVector<float>(100);
    QVERIFY(!vector1.isNull());
    QVERIFY(!vector1.isEmpty());
    QCOMPARE(vector1.size(), 100);
    QVERIFY(vector1.memoryId() != 0);
    QVERIFY(vector1.context() == &context);
    QVERIFY(!vector1.isMapped());

    for (int index = 0; index < 100; ++index)
        vector1[index] = float(index);
    QVERIFY(vector1.isMapped());

    vector1.unmap();
    QVERIFY(!vector1.isMapped());
    for (int index = 0; index < 100; ++index)
        QCOMPARE(vector1[index], float(index));
    QVERIFY(vector1.isMapped());

    QCLKernel addToVector = program.createKernel("addToVector");
    addToVector.setGlobalWorkSize(vector1.size());
    addToVector(vector1, 42.0f);
    QVERIFY(!vector1.isMapped());

    for (int index = 0; index < 100; ++index) {
        QCOMPARE(constVectorAt(vector1, index), float(index + 42));
        QCOMPARE(vector1[index], float(index + 42));
    }
    QVERIFY(vector1.isMapped());

    vector1.release();
    QVERIFY(vector1.isNull());
    QVERIFY(vector1.isEmpty());
    QCOMPARE(vector1.size(), 0);
    QVERIFY(vector1.memoryId() == 0);
    QVERIFY(vector1.context() == 0);
    QVERIFY(!vector1.isMapped());
}

void tst_QCL::eventProfiling()
{
    QCLCommandQueue queue = context.defaultCommandQueue();
    QVERIFY(!queue.isProfilingEnabled());
    queue.setProfilingEnabled(true);
    QVERIFY(queue.isProfilingEnabled());

    QCLVector<float> vector1 = context.createVector<float>(20000);
    for (int index = 0; index < vector1.size(); ++index)
        vector1[index] = float(index);
    vector1.unmap();

    QCLKernel addToVector = program.createKernel("addToVector");
    addToVector.setGlobalWorkSize(vector1.size());
    QCLEvent event = addToVector(vector1, 42.0f);

    event.waitForFinished();

    // The following tests assume that device time is monotonicly increasing.
    QVERIFY(event.finishTime() != 0);
    QVERIFY(event.submitTime() >= event.queueTime());
    QVERIFY(event.runTime() >= event.submitTime());
    QVERIFY(event.finishTime() >= event.runTime());

    queue.setProfilingEnabled(false);
    QVERIFY(!queue.isProfilingEnabled());
}

QTEST_MAIN(tst_QCL)

#include "tst_qcl.moc"
