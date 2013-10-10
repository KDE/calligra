#ifndef FAKE_KDEBUG_H
#define FAKE_KDEBUG_H

#include <QDebug>
#include <kglobal.h>

#ifdef DISABLE_KDEBUG
#include <QFile>
static QFile devnull{"/dev/null"};
#define Q_DEBUG(MsgType) QDebug(&devnull)
#else
#define Q_DEBUG(MsgType) QDebug(MsgType)
#endif

static inline QDebug kDebug(int area = 0)
{
    Q_UNUSED(area);
    return Q_DEBUG(QtDebugMsg);
}

static inline QDebug kDebug(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return Q_DEBUG(QtDebugMsg);
}

static inline QDebug kWarning(int area = 0)
{
    Q_UNUSED(area);
    return Q_DEBUG(QtWarningMsg);
}

static inline QDebug kWarning(bool cond, int area = 0)
{
    Q_UNUSED(cond);
    Q_UNUSED(area);
    return Q_DEBUG(QtWarningMsg);
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
