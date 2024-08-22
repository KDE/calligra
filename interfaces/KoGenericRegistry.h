/* This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2004 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_GENERIC_REGISTRY_H_
#define _KO_GENERIC_REGISTRY_H_

#include <QHash>
#include <QList>
#include <QString>

/**
 * Base class for registry objects.
 *
 * Registered objects are owned by the registry.
 *
 * Items are mapped by QString as a unique Id.
 *
 * Example of use:
 * @code
 * class KoMyClassRegistry : public KoGenericRegistry<MyClass*> {
 * public:
 *   static KoMyClassRegistry * instance();
 * private:
 *  static KoMyClassRegistry* s_instance;
 * };
 *
 * KoMyClassRegistry *KoMyClassRegistry::s_instance = 0;
 * KoMyClassRegistry * KoMyClassRegistry::instance()
 * {
 *    if(s_instance == 0)
 *    {
 *      s_instance = new KoMyClassRegistry;
 *    }
 *    return s_instance;
 * }
 *
 * @endcode
 */
template<typename T>
class KoGenericRegistry
{
public:
    KoGenericRegistry() = default;
    virtual ~KoGenericRegistry()
    {
        m_hash.clear();
    }

public:
    /**
     * Add an object to the registry. If it is a QObject, make sure it isn't in the
     * QObject ownership hierarchy, since the registry itself is responsible for
     * deleting it.
     *
     * @param item the item to add (NOTE: T must have an QString id() const   function)
     */
    void add(T item)
    {
        Q_ASSERT(item);
        QString id = item->id();
        if (m_hash.contains(id)) {
            m_doubleEntries << value(id);
            remove(id);
        }
        m_hash.insert(id, item);
    }

    /**
     * add an object to the registry
     * @param id the id of the object
     * @param item the item to add
     */
    void add(const QString &id, T item)
    {
        Q_ASSERT(item);
        if (m_hash.contains(id)) {
            m_doubleEntries << value(id);
            remove(id);
        }
        m_hash.insert(id, item);
    }

    /**
     * This function removes an item from the registry
     */
    void remove(const QString &id)
    {
        m_hash.remove(id);
    }

    /**
     * Retrieve the object from the registry based on the unique
     * identifier string.
     *
     * @param id the id
     */
    T get(const QString &id) const
    {
        return value(id);
    }

    /**
     * @return if there is an object stored in the registry identified
     * by the id.
     * @param id the unique identifier string
     */
    bool contains(const QString &id) const
    {
        return m_hash.contains(id);
    }

    /**
     * Retrieve the object from the registry based on the unique identifier string
     * @param id the id
     */
    const T value(const QString &id) const
    {
        return m_hash.value(id);
    }

    /**
     * @return a list of all keys
     */
    QList<QString> keys() const
    {
        return m_hash.keys();
    }

    int count() const
    {
        return m_hash.count();
    }

    QList<T> values() const
    {
        return m_hash.values();
    }

    QList<T> doubleEntries() const
    {
        return m_doubleEntries;
    }

private:
    QList<T> m_doubleEntries;

private:
    QHash<QString, T> m_hash;
};

#endif
