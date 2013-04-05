#ifndef FAKE_KRECENTDIRS_H
#define FAKE_KRECENTDIRS_H

#include <QString>
#include <QStringList>

namespace KRecentDirs
{
    static QStringList list(const QString &fileClass) { return QStringList(); }
    static QString dir(const QString &fileClass) { return QString(); }
    static void add(const QString &fileClass, const QString &directory) {}
}

#endif
