#ifndef FAKE_KDEBUG_H
#define FAKE_KDEBUG_H

#include <QDebug>
#include <kglobal.h>

static inline QDebug kDebug(int area = 0)
{
    Q_UNUSED(area);
    return QDebug(QtDebugMsg);
}

static inline QDebug kDebug(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return QDebug(QtDebugMsg);
}

static inline QDebug kWarning(int area = 0)
{
    Q_UNUSED(area);
    return QDebug(QtWarningMsg);
}

static inline QDebug kWarning(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return QDebug(QtWarningMsg);
}

static inline QDebug kError(int area = 0)
{
    Q_UNUSED(area);
    return QDebug(QtCriticalMsg);
}

static inline QDebug kError(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return QDebug(QtCriticalMsg);
}

static inline QDebug kFatal(int area = 0)
{
    Q_UNUSED(area);
    return QDebug(QtFatalMsg);
}

static inline QDebug kFatal(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return QDebug(QtFatalMsg);
}

#endif
