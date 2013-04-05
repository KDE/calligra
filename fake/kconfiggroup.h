#ifndef FAKE_KCONFIGGROUP_H
#define FAKE_KCONFIGGROUP_H

#include <QVariant>
#include <QStringList>
#include <QSharedPointer>
#include <QColor>
#include <QDebug>

#include <kconfigbase.h>
#include <ksharedconfig.h>

class KConfigGroup : public KConfigBase
{
public:
    KConfigGroup(const QString &path = QByteArray()) : KConfigBase(path) {}
    KConfigGroup(KConfigBase *master, const QString &group) : KConfigBase((master ? master->m_path + "/" : QString()) + group) {}
    KConfigGroup(KConfigBase *master, const char *group) : KConfigBase((master ? master->m_path + "/" : QString()) + QString::fromUtf8(group)) {}
    KConfigGroup(const KSharedConfigPtr &master, const QString &group) : KConfigBase((master ? master->m_path + "/" : QString()) + group.toUtf8()) {}
    KConfigGroup(const KSharedConfigPtr &master, const char *group) : KConfigBase((master ? master->m_path : QString()) + "/" + QString::fromUtf8(group)) {}

    bool isValid() const { return true; }

    QString name() const
    {
        QStringList l = m_path.split('/', QString::SkipEmptyParts);
        return l.isEmpty() ? QString() : l.last();
    }

    bool exists() const
    {
        QStringList parts = m_path.split('/', QString::SkipEmptyParts);
        if (parts.isEmpty()) return false;
        QSettings settings(parts.first());
        for(int i = 1; i < parts.count(); ++i) {
            if (!settings.childGroups().contains(parts[i]))
                return false;
            settings.beginGroup(parts[i]);
        }
        return true;
    }

    void sync() {}
    void markAsClean() {}

#if 0
    AccessMode accessMode() const;
    KConfig* config();
    const KConfig* config() const;
    void copyTo(KConfigBase *other, WriteConfigFlags pFlags = Normal) const;
    void reparent(KConfigBase *parent, WriteConfigFlags pFlags = Normal);
    KConfigGroup parent() const;
    QStringList groupList() const;
    QStringList keyList() const;
    void deleteGroup(WriteConfigFlags pFlags = Normal);
    using KConfigBase::deleteGroup;
    template <typename T>
        inline T readEntry(const QString &key, const T &aDefault) const
            { return readCheck(key.toUtf8().constData(), aDefault); }
    template <typename T>
        inline T readEntry(const char *key, const T &aDefault) const
            { return readCheck(key, aDefault); }
#endif

    QVariant readEntry(const QString &key, const QVariant &aDefault) const
    {
        qWarning() << Q_FUNC_INFO << "TODO" << key;
        return aDefault;
    }
    QVariant readEntry(const char *key, const QVariant &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    qreal readEntry(const QString &key, const double &aDefault) const
    {
        return readEntry(key, QVariant::fromValue<double>(aDefault)).toDouble();
    }
    qreal readEntry(const char *key, const double &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    quint32 readEntry(const QString &key, const quint32 &aDefault) const
    {
        return readEntry(key, QVariant::fromValue<uint>(aDefault)).toUInt();
    }
    quint32 readEntry(const char *key, const quint32 &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    int readEntry(const QString &key, const int &aDefault) const
    {
        return readEntry(key, QVariant::fromValue<int>(aDefault)).toInt();
    }
    int readEntry(const char *key, const int &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    bool readEntry(const QString &key, bool aDefault) const
    {
        return readEntry(key, QVariant::fromValue<bool>(aDefault)).toBool();
    }
    bool readEntry(const char *key, bool aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    QColor readEntry(const QString &key, const QColor &aDefault) const
    {
        return QColor(readEntry(key, aDefault.name()));
    }
    QColor readEntry(const char *key, const QColor &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    QString readEntry(const QString &key, const QString &aDefault = QString()) const
    {
        return readEntry(key, QVariant::fromValue<QString>(aDefault)).toString();
    }
    QString readEntry(const char *key, const QString &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }
    QString readEntry(const QString &key, const char *aDefault = 0) const
    {
        return readEntry(key, QString::fromUtf8(aDefault));
    }
    QString readEntry(const char *key, const char *aDefault = 0) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    QVariantList readEntry(const QString &key, const QVariantList &aDefault) const
    {
        return readEntry(key, QVariant(aDefault)).toList();
    }
    QVariantList readEntry(const char *key, const QVariantList &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    QStringList readEntry(const QString &key, const QStringList &aDefault) const
    {
        return readEntry(key, QVariant::fromValue<QStringList>(aDefault)).toStringList();
    }
    QStringList readEntry(const char *key, const QStringList &aDefault) const
    {
        return readEntry(QString::fromUtf8(key), aDefault);
    }

    QList<int> readEntry(const QString &key, const QList<int> &aDefault) const
    {
        QVariantList list;
        Q_FOREACH(const QVariant &v, aDefault)
            list.append(v.toInt());
        QList<int> result;
        list = readEntry(key, QVariant::fromValue<QVariantList>(list)).toList();
        Q_FOREACH(const QVariant &v, aDefault)
            result.append(v.toInt());
        return result;
    }

#if 0
    template<typename T>
        inline QList<T> readEntry(const QString &key, const QList<T> &aDefault) const
            { return readListCheck(key.toUtf8().constData(), aDefault); }
    template<typename T>
        inline QList<T> readEntry(const char *key, const QList<T> &aDefault) const
            { return readListCheck(key, aDefault); }
    QStringList readXdgListEntry(const QString &pKey, const QStringList &aDefault = QStringList()) const;
    QStringList readXdgListEntry(const char *pKey, const QStringList &aDefault = QStringList()) const;
#endif

    QString readPathEntry(const QString &pKey, const QString &aDefault) const
    {
        return readEntry(pKey, aDefault);
    }

    QString readPathEntry(const char *key, const QString &aDefault) const
    {
        return readEntry(key, aDefault);
    }

#if 0
    /**
     * Reads a list of paths
     *
     * Read the value of an entry specified by @p pKey in the current group
     * and interpret it as a list of paths. This means, dollar expansion is activated
     * for this value, so that e.g. $HOME gets expanded.
     *
     * @param pKey the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the list, or @p aDefault if the key does not exist
     */
    QStringList readPathEntry(const QString &pKey, const QStringList &aDefault) const;
    /** Overload for readPathEntry(const QString&, const QStringList&) */
    QStringList readPathEntry(const char *key, const QStringList &aDefault) const;

    /**
     * Reads an untranslated string entry
     *
     * You should not normally need to use this.
     *
     * @param pKey the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the value for this key, or @p aDefault if the key does not exist
     */
    QString readEntryUntranslated(const QString &pKey,
                                  const QString &aDefault = QString()) const;
    /** Overload for readEntryUntranslated(const QString&, const QString&) */
    QString readEntryUntranslated(const char *key,
                                  const QString &aDefault = QString()) const;

    /**
     * Writes a value to the configuration object.
     *
     * @param key the key to write to
     * @param value the value to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see readEntry(), writeXdgListEntry(), deleteEntry()
     */
#endif

    void writeEntry(const QString &key, const QVariant &value, WriteConfigFlags pFlags = Normal)
    {
        Q_UNUSED(pFlags);
        qWarning() << Q_FUNC_INFO << "TODO" << key << value;
    }

#if 0
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QVariant &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QString &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QString &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QByteArray &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QByteArray &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const char *value, WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const char *value, WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
        inline void writeEntry(const char *key, const T &value, WriteConfigFlags pFlags = Normal)
            { writeCheck( key, value, pFlags ); }
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
        inline void writeEntry(const QString &key, const T &value, WriteConfigFlags pFlags = Normal)
            { writeCheck( key.toUtf8().constData(), value, pFlags ); }
#endif

    void writeEntry(const QString &/*key*/, const QStringList &/*value*/, WriteConfigFlags pFlags = Normal)
    {
        Q_UNUSED(pFlags);
        //qWarning() << Q_FUNC_INFO << "TODO" << key << value;
    }
    
    void writeEntry(const QString &/*key*/, const QList<int> &/*value*/, WriteConfigFlags pFlags = Normal)
    {
        Q_UNUSED(pFlags);
        //qWarning() << Q_FUNC_INFO << "TODO" << key << value;
    }

#if 0
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QStringList &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QVariantList &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QVariantList &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
        inline void writeEntry(const QString &key, const QList<T> &value, WriteConfigFlags pFlags = Normal)
            { writeListCheck( key.toUtf8().constData(), value, pFlags ); }
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
        inline void writeEntry(const char *key, const QList<T> &value, WriteConfigFlags pFlags = Normal)
            { writeListCheck( key, value, pFlags ); }

    /**
     * Writes a list of strings to the config object, following XDG
     * desktop entry spec separator semantics
     *
     * @param pKey the key to write to
     * @param value the list to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see writeEntry(), readXdgListEntry()
     */
    void writeXdgListEntry(const QString &pKey, const QStringList &value,
                           WriteConfigFlags pFlags = Normal);
    /** Overload for writeXdgListEntry(const QString&, const QStringList&, WriteConfigFlags) */
    void writeXdgListEntry(const char *pKey, const QStringList &value,
                           WriteConfigFlags pFlags = Normal);
#endif

    void writePathEntry(const QString &pKey, const QString &path, WriteConfigFlags pFlags = Normal)
    {
        writeEntry(pKey, path, pFlags);
    }
    void writePathEntry(const char *pKey, const QString &path, WriteConfigFlags pFlags = Normal)
    {
        writeEntry(pKey, path, pFlags);
    }

#if 0
    /**
     * Writes a list of paths to the configuration
     *
     * If any of the paths are located under $HOME, the user's home directory
     * is replaced with $HOME in the persistent storage.
     * The paths should therefore be read back with readPathEntry()
     *
     * @param pKey the key to write to
     * @param value the list to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see writeEntry(), readPathEntry()
     */
    void writePathEntry(const QString &pKey, const QStringList &value,
                        WriteConfigFlags pFlags = Normal);
    /** Overload for writePathEntry(const QString&, const QStringList&, WriteConfigFlags) */
    void writePathEntry(const char *pKey, const QStringList &value,
                        WriteConfigFlags pFlags = Normal);

    /**
     * Deletes the entry specified by @p pKey in the current group
     *
     * This also hides system wide defaults.
     *
     * @param pKey the key to delete
     * @param pFlags the flags to use when deleting this entry
     *
     * @see deleteGroup(), readEntry(), writeEntry()
     */
    void deleteEntry(const QString &pKey, WriteConfigFlags pFlags = Normal);
    /** Overload for deleteEntry(const QString&, WriteConfigFlags) */
    void deleteEntry(const char *pKey, WriteConfigFlags pFlags = Normal);

    /**
     * Checks whether the key has an entry in this group
     *
     * Use this to determine if a key is not specified for the current
     * group (hasKey() returns false).
     *
     * If this returns @c false for a key, readEntry() (and its variants)
     * will return the default value passed to them.
     *
     * @param key the key to search for
     * @return @c true if the key is defined in this group by any of the
     *         configuration sources, @c false otherwise
     *
     * @see readEntry()
     */
#endif

    bool hasKey(const QString &key) const
    {
        qWarning() << Q_FUNC_INFO << "TODO" << key;
        return false;
    }

#if 0
    /** Overload for hasKey(const QString&) const */
    bool hasKey(const char *key) const;

    /**
     * Whether this group may be changed
     *
     * @return @c false if the group may be changed, @c true otherwise
     */
    bool isImmutable() const;

    /**
     * Checks if it is possible to change the given entry
     *
     * If isImmutable() returns @c true, then this method will return
     * @c true for all inputs.
     *
     * @param key the key to check
     * @return @c false if the key may be changed using this configuration
     *         group object, @c true otherwise
     */
    bool isEntryImmutable(const QString &key) const;
    /** Overload for isEntryImmutable(const QString&) const */
    bool isEntryImmutable(const char *key) const;

    /**
     * Reverts an entry to the default settings.
     *
     * Reverts the entry with key @p key in the current group in the
     * application specific config file to either the system wide (default)
     * value or the value specified in the global KDE config file.
     *
     * To revert entries in the global KDE config file, the global KDE config
     * file should be opened explicitly in a separate config object.
     *
     * @note This is @em not the same as deleting the key, as instead the
     * global setting will be copied to the configuration file that this
     * object manipulates.
     *
     * @param key The key of the entry to revert.
     */
    void revertToDefault(const QString &key);
    /** Overload for revertToDefault(const QString&) */
    void revertToDefault(const char* key);

    /**
     * Whether a default is specified for an entry in either the
     * system wide configuration file or the global KDE config file
     *
     * If an application computes a default value at runtime for
     * a certain entry, e.g. like:
     * \code
     * QColor computedDefault = qApp->palette().color(QPalette::Active, QPalette::Text)
     * QColor color = config->readEntry(key, computedDefault);
     * \endcode
     * then it may wish to make the following check before
     * writing back changes:
     * \code
     * if ( (value == computedDefault) && !config->hasDefault(key) )
     *    config->revertToDefault(key)
     * else
     *    config->writeEntry(key, value)
     * \endcode
     *
     * This ensures that as long as the entry is not modified to differ from
     * the computed default, the application will keep using the computed default
     * and will follow changes the computed default makes over time.
     *
     * @param key the key of the entry to check
     * @return @c true if the global or system settings files specify a default
     *          for @p key in this group, @c false otherwise
     */
    bool hasDefault(const QString &key) const;
    /** Overload for hasDefault(const QString&) const */
    bool hasDefault(const char *key) const;

    /**
     * Returns a map (tree) of entries for all entries in this group
     *
     * Only the actual entry string is returned, none of the
     * other internal data should be included.
     *
     * @return a map of entries in this group, indexed by key
     */
    QMap<QString, QString> entryMap() const;

protected:
    bool hasGroupImpl(const QByteArray &group) const;
    KConfigGroup groupImpl(const QByteArray &b);
    const KConfigGroup groupImpl(const QByteArray &b) const;
    void deleteGroupImpl(const QByteArray &group, WriteConfigFlags flags);
    bool isGroupImmutableImpl(const QByteArray &aGroup) const;

private:
    QExplicitlySharedDataPointer<KConfigGroupPrivate> d;

    template<typename T>
    inline T readCheck(const char *key, const T &defaultValue) const;

    template<typename T>
    inline QList<T> readListCheck(const char *key, const QList<T> &defaultValue) const;

    template<typename T>
    inline void writeCheck(const char *key, const T &value, WriteConfigFlags pFlags);

    template<typename T>
    inline void writeListCheck(const char *key, const QList<T> &value, WriteConfigFlags pFlags);

    friend class KConfigGroupPrivate;

    /**
     * Return the data in @p value converted to a QVariant
     *
     * @param pKey the name of the entry being converted, this is only used for error
     * reporting
     * @param value the UTF-8 data to be converted
     * @param aDefault the default value if @p pKey is not found
     * @return @p value converted to QVariant, or @p aDefault if @p value is invalid or cannot be converted.
     */
    static QVariant convertToQVariant(const char *pKey, const QByteArray &value, const QVariant &aDefault);
    friend class KServicePrivate; // XXX yeah, ugly^5

#endif

};

#endif
