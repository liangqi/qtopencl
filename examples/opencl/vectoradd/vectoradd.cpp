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
#include "qclcontext.h"

int input1[2048];
int input2[2048];
int output[2048];

int main(int, char **)
{
    // Create the OpenCL context for the default GPU device.
//! [1]
    QCLContext context;
    if (!context.create()) {
        fprintf(stderr, "Could not create OpenCL context for the GPU\n");
        return 1;
    }
//! [1]

    // Construct the input vectors to pass to the kernel.
//! [2]
    for (int index = 0; index < 2048; ++index) {
        input1[index] = index;
        input2[index] = 2048 - index;
    }
//! [2]

    // Create OpenCL buffer objects for the input and output vectors.
//! [3]
    QCLBuffer buffer1 = context.createBufferCopy(input1, sizeof(input1), QCLBuffer::ReadOnly);
    QCLBuffer buffer2 = context.createBufferCopy(input2, sizeof(input2), QCLBuffer::ReadOnly);
//! [3]
//! [4]
    QCLBuffer buffer3 = context.createBufferDevice(sizeof(output), QCLMemoryObject::WriteOnly);
//! [4]

    // Build the program and locate the entry point kernel.
//! [5]
    static const char vectorAddSource[] =
        "__kernel void vectorAdd(__global __read_only int *input1,\n"
        "                        __global __read_only int *input2,\n"
        "                        __global __write_only int *output)\n"
        "{\n"
        "    unsigned int index = get_global_id(0);\n"
        "    output[index] = input1[index] + input2[index];\n"
        "}\n"
    ;
    QCLProgram program = context.buildProgramFromSourceCode(vectorAddSource);
    QCLKernel kernel = program.createKernel("vectorAdd");
//! [5]

    // Execute the kernel and then read back the results.
//! [6]
    kernel.setGlobalWorkSize(2048);
//! [6]
//! [7]
    kernel(buffer1, buffer2, buffer3);
//! [7]
//! [8]
    buffer3.read(output, sizeof(output));
//! [8]

    // Check the answer.
    for (int index = 0; index < 2048; ++index) {
        if (output[index] != 2048) {
            fprintf(stderr, "Answer at index %d is %d, should be %d\n",
                    index, output[index], 2048);
            return 1;
        }
    }
    printf("Answer is correct: %d\n", 2048);

    return 0;
}
