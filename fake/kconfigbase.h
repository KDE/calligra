#ifndef FAKE_KCONFIGBASE_H
#define FAKE_KCONFIGBASE_H

#include <QVariant>
#include <QStringList>
#include <QSharedPointer>
#include <QDebug>
#include "kdecore_export.h"

class KConfigGroup;

class KDECORE_EXPORT KConfigBase
{
protected:
    KConfigBase(const QString &path = QByteArray());
    
public:
    virtual ~KConfigBase() {}

    enum WriteConfigFlag { Persistent = 0x01, Global = 0x02, Localized = 0x04, Normal=Persistent };
    Q_DECLARE_FLAGS(WriteConfigFlags, WriteConfigFlag)

    virtual QStringList groupList() const;

    bool hasGroup(const QString &group) const;
    bool hasGroup(const char *group) const;
    bool hasGroup(const QByteArray &group) const;

    KConfigGroup group(const QByteArray &group);
    KConfigGroup group(const QString &group);
    KConfigGroup group(const char *group);

    const KConfigGroup group(const QByteArray &group) const;
    const KConfigGroup group(const QString &group) const;
    const KConfigGroup group(const char *group) const;

#if 0
    /**
     * Delete @p aGroup. This marks @p aGroup as @em deleted in the config object. This effectively
     * removes any cascaded values from config files earlier in the stack.
     */
    void deleteGroup(const QByteArray &group, WriteConfigFlags flags = Normal);
    void deleteGroup(const QString &group, WriteConfigFlags flags = Normal);
    void deleteGroup(const char *group, WriteConfigFlags flags = Normal);
#endif

    virtual void sync() {}

#if 0
    /**
     * Reset the dirty flags of all entries in the entry map, so the
     * values will not be written to disk on a later call to sync().
     */
    virtual void markAsClean() = 0;

    /**
     * Possible return values for accessMode().
     */
    enum AccessMode { NoAccess, ReadOnly, ReadWrite };

    /**
     * Returns the access mode of the app-config object.
     *
     * Possible return values
     * are NoAccess (the application-specific config file could not be
     * opened neither read-write nor read-only), ReadOnly (the
     * application-specific config file is opened read-only, but not
     * read-write) and ReadWrite (the application-specific config
     * file is opened read-write).
     *
     * @return the access mode of the app-config object
     */
    virtual AccessMode accessMode() const = 0;

    /**
     * Checks whether this configuration object can be modified.
     * @return whether changes may be made to this configuration object.
     */
    virtual bool isImmutable() const = 0;

    /**
     * Can changes be made to the entries in @p aGroup?
     * 
     * @param aGroup The group to check for immutability.
     * @return @c false if the entries in @p aGroup can be modified.
     */
    bool isGroupImmutable(const QByteArray& aGroup) const;
    bool isGroupImmutable(const QString& aGroup) const;
    bool isGroupImmutable(const char *aGroup) const;

#endif

public:
    QString m_path;
};

#endif
