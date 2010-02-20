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
#include <QtCore/qfuture.h>

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

    bool isQueued() const { return status() == CL_QUEUED; }
    bool isSubmitted() const { return status() == CL_SUBMITTED; }
    bool isRunning() const { return status() == CL_RUNNING; }
    bool isFinished() const { return status() == CL_COMPLETE; }

    void waitForFinished();

    bool operator==(const QCLEvent &other) const;
    bool operator!=(const QCLEvent &other) const;

    QFuture<void> toFuture() const;
    operator QFuture<void>() const;

private:
    cl_event m_id;

    int status() const;
};

class Q_CL_EXPORT QCLEventList
{
public:
    QCLEventList() {}
    QCLEventList(const QCLEvent &event);
    QCLEventList(const QCLEventList &other);
    ~QCLEventList();

    QCLEventList &operator=(const QCLEventList &other);

    bool isEmpty() const { return m_events.isEmpty(); }
    int size() const { return m_events.size(); }

    void append(const QCLEvent &event);
    void append(const QCLEventList &other);
    void remove(const QCLEvent &event);

    QCLEvent at(int index) const;
    bool contains(const QCLEvent &event) const;

    const cl_event *eventData() const;

    QCLEventList &operator+=(const QCLEvent &event);
    QCLEventList &operator+=(const QCLEventList &other);

    QCLEventList &operator<<(const QCLEvent &event);
    QCLEventList &operator<<(const QCLEventList &other);

    void waitForFinished();

    QFuture<void> toFuture() const;
    operator QFuture<void>() const;

private:
    QVector<cl_event> m_events;
};

inline bool QCLEvent::operator==(const QCLEvent &other) const
{
    return m_id == other.m_id;
}

inline bool QCLEvent::operator!=(const QCLEvent &other) const
{
    return m_id != other.m_id;
}

inline bool QCLEventList::contains(const QCLEvent &event) const
{
    return m_events.contains(event.id());
}

inline const cl_event *QCLEventList::eventData() const
{
    return m_events.isEmpty() ? 0 : m_events.constData();
}

inline QCLEventList &QCLEventList::operator+=(const QCLEvent &event)
{
    append(event);
    return *this;
}

inline QCLEventList &QCLEventList::operator+=(const QCLEventList &other)
{
    append(other);
    return *this;
}

inline QCLEventList &QCLEventList::operator<<(const QCLEvent &event)
{
    append(event);
    return *this;
}

inline QCLEventList &QCLEventList::operator<<(const QCLEventList &other)
{
    append(other);
    return *this;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
