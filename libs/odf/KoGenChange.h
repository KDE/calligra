/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pierre.stirnweiss_calligra@gadz.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOGENCHANGE_H
#define KOGENCHANGE_H

#include "koodf_export.h"
#include <QMap>
#include <QString>

#include <OdfDebug.h>

class KoGenChanges;
class KoXmlWriter;

/**
 * A generic change, i.e. basically a collection of properties and a name.
 * Instances of KoGenChange can either be held in the KoGenChanges collection,
 * or created (e.g. on the stack) and given to KoGenChanges::insert.
 *
 * Derived from code from KoGenStyle
 */
class KOODF_EXPORT KoGenChange
{
public:
    /**
     * Possible values for the "type" of the KoGenChange.
     * If there is a still missing add it here so that it is possible to use the same
     * saving code in all applications.
     */
    enum Type {
        InsertChange,
        FormatChange,
        DeleteChange,
        UNKNOWN = 9999
    };

    enum ChangeFormat {
        ODF_1_2,
        DELTAXML
    };

    /**
     * Start the definition of a new change. Its name will be set later by KoGenChanges::insert(),
     * but first you must define its properties and attributes.
     *
     */
    explicit KoGenChange(KoGenChange::ChangeFormat changeFormat = KoGenChange::ODF_1_2);
    ~KoGenChange();

    /// Set the type of this change
    void setType(KoGenChange::Type type)
    {
        m_type = type;
    }

    /// set the format to be used to save changes
    void setChangeFormat(KoGenChange::ChangeFormat changeFormat)
    {
        m_changeFormat = changeFormat;
    }

    /// Return the type of this style
    Type type() const
    {
        return m_type;
    }

    /// Return the format to be used to save changes
    KoGenChange::ChangeFormat changeFormat() const
    {
        return m_changeFormat;
    }

    /// Add a property to the style
    void addChangeMetaData(const QString &propName, const QString &propValue)
    {
        m_changeMetaData.insert(propName, propValue);
    }

    /// Overloaded version of addProperty that takes a char*, usually for "..."
    void addChangeMetaData(const QString &propName, const char *propValue)
    {
        m_changeMetaData.insert(propName, propValue);
    }
    /// Overloaded version of addProperty that converts an int to a string
    void addChangeMetaData(const QString &propName, int propValue)
    {
        m_changeMetaData.insert(propName, QString::number(propValue));
    }
    /// Overloaded version of addProperty that converts a bool to a string (false/true)
    void addChangeMetaData(const QString &propName, bool propValue)
    {
        m_changeMetaData.insert(propName, propValue ? "true" : "false");
    }

    /**
     * @brief Add a child element to the properties.
     *
     * What is meant here is that the contents of the QString
     * will be written out literally. This means you should use
     * KoXmlWriter to generate it:
     * @code
     * QBuffer buffer;
     * buffer.open( QIODevice::WriteOnly );
     * KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
     * elementWriter.startElement( "..." );
     * ...
     * elementWriter.endElement();
     * QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
     * gs.addChildElement( "...", elementContents );
     * @endcode
     *
     * The value of @p elementName isn't used, except that it must be unique.
     */
    void addChildElement(const QString &elementName, const QString &elementContents)
    {
        m_literalData.insert(elementName, elementContents);
    }

    /**
     *  Write the definition of this change to @p writer, using the OASIS format.
     *  @param writer the KoXmlWriter in which the element will be created and filled in
     *  @param name must come from the collection.
     */
    void writeChange(KoXmlWriter *writer, const QString &name) const;

    /**
     *  QMap requires a complete sorting order.
     *  Another solution would have been a qdict and a key() here, a la KoTextFormat,
     *  but the key was difficult to generate.
     *  Solutions with only a hash value (not representative of the whole data)
     *  require us to write a hashtable by hand....
     */
    bool operator<(const KoGenChange &other) const;

    /// Not needed for QMap, but can still be useful
    bool operator==(const KoGenChange &other) const;

private:
    QString changeMetaData(const QString &propName) const
    {
        QMap<QString, QString>::const_iterator it = m_changeMetaData.find(propName);
        if (it != m_changeMetaData.end())
            return it.value();
        return QString();
    }

    void writeChangeMetaData(KoXmlWriter *writer) const;

    void writeODF12Change(KoXmlWriter *writer, const QString &name) const;

    void writeDeltaXmlChange(KoXmlWriter *writer, const QString &name) const;

private:
    // Note that the copy constructor and assignment operator are allowed.
    // Better not use pointers below!
    ChangeFormat m_changeFormat;
    Type m_type;
    /// We use QMaps since they provide automatic sorting on the key (important for unicity!)
    QMap<QString, QString> m_changeMetaData;
    QMap<QString, QString> m_literalData;
};

#endif /* KOGENCHANGE_H */
