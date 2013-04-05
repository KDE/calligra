#ifndef FAKE_KDESKTOPFILE_H
#define FAKE_KDESKTOPFILE_H

#include <QFile>

#include <kconfig.h>
#include <kconfiggroup.h>

class KDesktopFile : public KConfig
{
public:
    KDesktopFile(const QString &file = QString()) : KConfig(file) {}
    KConfigGroup desktopGroup() const { return KConfigGroup("Desktop Entry"); }

    QString readType() const { return desktopGroup().readEntry("Type"); }
    QString readIcon() const { return desktopGroup().readEntry("Icon"); }
    QString readName() const { return desktopGroup().readEntry("Name"); }
    QString readComment() const { return desktopGroup().readEntry("Comment"); }
    QString readGenericName() const { return desktopGroup().readEntry("GenericName"); }
    QString readPath() const { return desktopGroup().readEntry("Path"); }
    QString readUrl() const { return desktopGroup().readEntry("URL"); }

    static bool isDesktopFile(const QString &filePath)
    {
        if (!filePath.endsWith(".desktop"))
            return false;
        QFile f(filePath);
        if (!f.exists())
            return false;
        return true;
    }

private:
};

#endif
