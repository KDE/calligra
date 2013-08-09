#include "kcomponentdata.h"

#include <ksharedconfig.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include <QStandardPaths>

using namespace KGlobal;

KSharedConfigPtr KComponentData::config() const
{
    return KGlobal::config();
}

KStandardDirs* KComponentData::dirs() const
{
    return KGlobal::dirs();
}

bool KComponentData::isValid() const
{
    return true;
}

KSharedConfigPtr KGlobal::config()
{
    static KSharedConfigPtr globalConfig;
    if (!globalConfig)
        globalConfig = KSharedConfig::openConfig(qApp->applicationName());
    return globalConfig;
}

KStandardDirs* KGlobal::dirs()
{
    static KStandardDirs *componentsDirs = 0;
    if (!componentsDirs)
        componentsDirs = new KStandardDirs();
    return componentsDirs;
}

static KLocale *s_locale = 0;

KLocale* KGlobal::locale()
{
    if (!s_locale)
        s_locale = new KLocale();
    return s_locale;
}

void KGlobal::setLocale(KLocale* locale)
{
    delete s_locale;
    s_locale = locale;
}

QString KGlobal::staticQString(const QByteArray &b)
{
    static QSet<QString> strings;
    QString s = QString::fromUtf8(b);
    QSet<QString>::ConstIterator it = strings.constFind(s);
    if (it == strings.constEnd())
        it = strings.insert(s);
    return *it;
}
