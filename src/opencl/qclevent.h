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

#ifndef QCLEVENT_H
#define QCLEVENT_H

#include "qclglobal.h"
#include <QtCore/qvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class Q_CL_EXPORT QCLEvent
{
public:
    QCLEvent() : m_id(0) {}
    QCLEvent(cl_event id) : m_id(id) {}
    QCLEvent(const QCLEvent &other);
    ~QCLEvent();

    QCLEvent &operator=(const QCLEvent &other);

    bool isNull() const { return m_id == 0; }

    cl_event id() const { return m_id; }

    int status() const;

    bool isQueued() const { return status() == CL_QUEUED; }
    bool isSubmitted() const { return status() == CL_SUBMITTED; }
    bool isRunning() const { return status() == CL_RUNNING; }
    bool isComplete() const { return status() == CL_COMPLETE; }

    void wait();
    static void waitForEvents(const QVector<QCLEvent> &events);

private:
    cl_event m_id;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
