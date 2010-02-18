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

int main(int, char **)
{
//! [1]
    QCLContext context;
    if (!context.create()) {
        fprintf(stderr, "Could not create OpenCL context for the GPU\n");
        return 1;
    }
//! [1]

//! [2]
    QCLVector<int> input1 = context.createVector<int>(2048);
    QCLVector<int> input2 = context.createVector<int>(2048);
    for (int index = 0; index < 2048; ++index) {
        input1[index] = index;
        input2[index] = 2048 - index;
    }
//! [2]
//! [3]
    QCLVector<int> output = context.createVector<int>(2048);
//! [3]

//! [4]
    QCLProgram program = context.buildProgramFromSourceFile(":/vectoradd.cl");
    QCLKernel kernel = program.createKernel("vectorAdd");
//! [4]

//! [5]
    kernel.setGlobalWorkSize(2048);
//! [5]
//! [6]
    kernel(input1, input2, output);
//! [6]

//! [7]
    for (int index = 0; index < 2048; ++index) {
        if (output[index] != 2048) {
            fprintf(stderr, "Answer at index %d is %d, should be %d\n",
                    index, output[index], 2048);
            return 1;
        }
    }
    printf("Answer is correct: %d\n", 2048);
//! [7]

    return 0;
}
