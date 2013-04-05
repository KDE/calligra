#include "kcomponentdata.h"

#include <ksharedconfig.h>
#include <kstandarddirs.h>

using namespace KGlobal;

KSharedConfigPtr KComponentData::config() const
{
    return m_sharedConfig;
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
        globalConfig = KSharedConfigPtr(new KSharedConfig(qApp->applicationName()));
    return globalConfig;
}

KStandardDirs* KGlobal::dirs()
{
    static KStandardDirs *componentsDirs = 0;
    if (!componentsDirs)
        componentsDirs = new KStandardDirs();
    return componentsDirs;
}

KLocale* KGlobal::locale()
{
    static KLocale *s_locale = 0;
    if (!s_locale)
        s_locale = new KLocale();
    return s_locale;
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
