/*
   SPDX-FileCopyrightText: 2006-2007 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_PROPERTIES_H
#define _KO_PROPERTIES_H

#include "kowidgetutils_export.h"
#include <QMap>
#include <QString>
#include <QVariant>

class QDomElement;

/**
 * A KoProperties is the (de-)serializable representation of
 * a key-value map. The serialisation format is XML.
 */
class KOWIDGETUTILS_EXPORT KoProperties
{
public:
    /**
     * Create a new properties object
     */
    KoProperties();

    /**
     * Copy constructor
     */
    KoProperties(const KoProperties &other);

    ~KoProperties();

public:
    /**
     * Fill the properties object from the XML dom node.
     *
     * load() does not touch existing properties if loading fails.
     *
     * @param root the root node of the properties subtree.
     */
    void load(const QDomElement &root);

    /**
     * Fill the properties object from the XML encoded
     * representation in string.
     *
     * load() does not touch existing properties if loading fails.
     *
     * @param string the stored properties.
     * @return false if loading failing, true if it succeeded
     */
    bool load(const QString &string);

    /**
     * Returns an iterator over the properties. The iterator is not
     * suitable for adding or removing properties.
     */
    QMapIterator<QString, QVariant> propertyIterator() const;

    /**
     * @return true if this KoProperties object does not contain any
     * properties.
     */
    bool isEmpty() const;

    /**
     * @brief Create a serialized version of these properties (as XML) with root as the root element.
     * @param root as the root element in the generated XML.
     */
    QString store(const QString &root) const;

    void save(QDomElement &root) const;

    /**
     * Set the property with name to value.
     */
    void setProperty(const QString &name, const QVariant &value);

    /**
     * Set value to the value associated with property name
     * @return false if the specified property did not exist.
     */
    bool property(const QString &name, QVariant &value) const;

    /**
     * Return a property by name, wrapped in a QVariant.
     * A typical usage:
     *  @code
     *      KoProperties *props = new KoProperties();
     *      props->setProperty("name", "Marcy");
     *      props->setProperty("age", 25);
     *      QString name = props->property("name").toString();
     *      int age = props->property("age").toInt();
     *  @endcode
     * @return a property by name, wrapped in a QVariant.
     * @param name the name (or key) with which the variant was registered.
     * @see intProperty() stringProperty()
     */
    QVariant property(const QString &name) const;

    /**
     * Return an integer property by name.
     * A typical usage:
     *  @code
     *      KoProperties *props = new KoProperties();
     *      props->setProperty("age", 25);
     *      int age = props->intProperty("age");
     *  @endcode
     * @return an integer property by name
     * @param name the name (or key) with which the variant was registered.
     * @param defaultValue the default value, should there not be any property by the name this will be returned.
     * @see property() stringProperty()
     */
    int intProperty(const QString &name, int defaultValue = 0) const;

    /**
     * Return a qreal property by name.
     * @param name the name (or key) with which the variant was registered.
     * @param defaultValue the default value, should there not be any property by the name this will be returned.
     */
    qreal doubleProperty(const QString &name, qreal defaultValue = 0.0) const;

    /**
     * Return a boolean property by name.
     * @param name the name (or key) with which the variant was registered.
     * @param defaultValue the default value, should there not be any property by the name this will be returned.
     */
    bool boolProperty(const QString &name, bool defaultValue = false) const;

    /**
     * Return an QString property by name.
     * A typical usage:
     *  @code
     *      KoProperties *props = new KoProperties();
     *      props->setProperty("name", "Marcy");
     *      QString name = props->stringProperty("name");
     *  @endcode
     * @return an QString property by name
     * @param name the name (or key) with which the variant was registered.
     * @see property() intProperty()
     * @param defaultValue the default value, should there not be any property by the name this will be returned.
     */
    QString stringProperty(const QString &name, const QString &defaultValue = QString()) const;

    /**
     * Returns true if the specified key is present in this properties
     * object.
     */
    bool contains(const QString &key) const;

    /**
     * Returns the value associated with the specified key if this
     * properties object contains the specified key; otherwise return
     * an empty QVariant.
     */
    QVariant value(const QString &key) const;

    bool operator==(const KoProperties &other) const;
    KoProperties operator=(const KoProperties &other) const;

private:
    class Private;
    Private *const d;
};

#endif // _KO_PROPERTIES_H
