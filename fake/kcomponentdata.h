#ifndef FAKE_KCOMPONENTDATA_H
#define FAKE_KCOMPONENTDATA_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QPrinter>
#include <QApplication>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include <ksharedconfig.h>
#include <kconfig.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kofake_export.h>

#define FAKE_KAPPLICATION_CTOR Q_ASSERT(false);
#define FAKE_KMAINWINDOW_CTOR Q_ASSERT(false);

class KStandardDirs;

class KOFAKE_EXPORT KComponentData
{
public:
    KComponentData(KAboutData *about = 0) { if(about) m_about = *about; }
    KSharedConfigPtr config() const;
    KStandardDirs* dirs() const;
    bool isValid() const;
    QString componentName() const { return  qApp->applicationName(); }
    const KAboutData* aboutData() const { return &m_about; }
private:
    KSharedConfigPtr m_sharedConfig;
    KAboutData m_about;
};

#define K_GLOBAL_STATIC(clazz, inst) \
    class KGlobalStaticWrapperPtr { \
    public: \
        mutable clazz *m_data; \
        KGlobalStaticWrapperPtr() : m_data(0) {} \
        ~KGlobalStaticWrapperPtr() { delete m_data; } \
        clazz* operator ->() { if (!m_data) m_data = new clazz(); return m_data; } \
        operator clazz*() { if (!m_data) m_data = new clazz(); return m_data; } \
        bool exists() const { return m_data; } \
    }; \
    static KGlobalStaticWrapperPtr inst;

namespace KGlobal {

    static KComponentData mainComponent()
    {
        static KComponentData mainComponentData;
        return mainComponentData;
    }

    static bool hasMainComponent()
    {
        return true;
    }

    static void setActiveComponent(KComponentData) {}
    static KComponentData activeComponent() { return mainComponent(); }

    KOFAKE_EXPORT KSharedConfigPtr config();
    KOFAKE_EXPORT KStandardDirs* dirs();
    KOFAKE_EXPORT KLocale* locale();

    KOFAKE_EXPORT QString staticQString(const QByteArray &b);
}

#endif
