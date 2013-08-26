#ifndef FAKE_KSERVICE_H
#define FAKE_KSERVICE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QDebug>

#include <ksharedptr.h>
#include <kurl.h>
#include <klibrary.h>

class KService : public KShared
{
public:
    typedef KSharedPtr<KService>Ptr;
    typedef QList<Ptr> List;

    QVariant property(const QString &) const { return QVariant(); }
    QString library() const { return QString(); }
    QString entryPath() const { return QString(); }
    QString path() const { return QString(); }
    QString name() const { return QString(); }
    QString icon() const { return QString(); }
    QString comment() const { return QString(); }
    QString genericName() const { return QString(); }
    QString untranslatedGenericName() const { return QString(); }
    QStringList keywords() const { return QStringList(); }
    QStringList categories() const { return QStringList(); }
    QStringList mimeTypes() const { return QStringList(); }

    static Ptr serviceByName( const QString& _name ) { return Ptr(); }
    static Ptr serviceByDesktopPath( const QString& _path ) { return Ptr(); }
    static Ptr serviceByDesktopName( const QString& _name ) { return Ptr(); }
    static Ptr serviceByMenuId( const QString& _menuId ) { return Ptr(); }
    static Ptr serviceByStorageId( const QString& _storageId ) { return Ptr(); }
    static List allServices() { return List(); }
    QStringList serviceTypes() const { return QStringList(); }

    bool noDisplay() const { return false; }

    template<class T>
    T* createInstance(QObject*, const QVariantList &args = QVariantList(), QString *error = 0) const
    {
        Q_UNUSED(args);
        Q_UNUSED(error);
        qWarning() << Q_FUNC_INFO << "TODO";
        return 0;
    }
};

#endif
