/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KOGENSTYLES_H
#define KOGENSTYLES_H

#include <qdict.h>
#include <qmap.h>

class KoXmlWriter;
class KoGenStyle;

/**
 * Repository of styles used during saving of OASIS/OOo file.
 * Each instance of KoGenStyles is a collection of styles whose names
 * are in the same "namespace".
 * This means there should be one instance for all styles in <office:styles>,
 * and automatic-styles, another instance for number formats, another
 * one for draw styles, and another one for list styles.
 *
 * "Style" in this context only means "a collection of properties".
 * The "Gen" means both "Generic" and "Generated" :)
 *
 * The basic design principle is the flyweight pattern: if you need
 * a style with the same properties from two different places, you
 * get the same object. Here it means rather you get the same name for the style,
 * and it will get saved only once to the file.
 *
 * KoGenStyles features sharing, creation on demand, and name generation.
 * Since this is used for saving only, it doesn't feature refcounting, nor
 * removal of individual styles.
 *
 * @author David Faure <faure@kde.org>
 */
class KoGenStyles
{
public:
    KoGenStyles();
    ~KoGenStyles();

    /**
     * Look up a style in the collection, inserting it if necessary.
     * This assigns a name to the style and returns it.
     *
     * @param name proposed (base) name for the style. Note that with the OASIS format,
     * the style name is never shown to the user (there's a separate display-name
     * attribute for that). So there are little reasons to use named styles anyway.
     * But this attribute can be used for clarity of the files.
     * If this name is already in use (for another style), then a number is appended
     * to it until unique.
     * @param forceNumbering if true, the generated style names will look like "name1", "name2".
     * If false, the first name that will be tried is "name". Set it to false if @p name
     * is supposed to be the full style name.
     *
     * @return the name for this style
     */
    QString lookup( const KoGenStyle& style, const QString& name = QString::null, bool forceNumbering = true );

    typedef QMap<KoGenStyle, QString> StyleMap;
    /**
     * Return the entire collection of styles
     * Use this for saving the styles
     */
    const StyleMap& styles() const { return m_styles; }

private:
    QString makeUniqueName( const QString& base, bool forceNumbering ) const;

    /// style definition -> name
    StyleMap m_styles;

    /// name -> style   (only used to check for name uniqueness)
    typedef QMap<QString, bool /*KoGenStyle*/> NameMap;
    NameMap m_names;
};

/**
 * A generic style, i.e. basically a collection of properties and a name.
 * Instances of KoGenStyle can either be held in the KoGenStyles collection,
 * or created (e.g. on the stack) and given to KoGenStyles::lookup.
 *
 * @author David Faure <faure@kde.org>
 */
class KoGenStyle
{
public:
    /// Create an automatic style. Its name will be set by KoGenStyles::lookup()
    KoGenStyle() {}
    /// Create an automatic style that inherits from another one.
    KoGenStyle( const QString& parentName ) : m_parentName( parentName ) {}

    /// Return the name of style's parent, if set
    QString parentName() const { return m_parentName; }

    /// Add a property to the style
    void setProperty( const QString& propName, const QString& propValue ) {
        m_properties.insert( propName, propValue );
    }

    /// Add an attribute to the style
    /// The difference between property and attributes is a bit oasis-format-specific:
    /// attributes are for the style element itself, and properties are in the style:properties child element
    void setAttribute( const QString& attrName, const QString& attrValue ) {
        m_attributes.insert( attrName, attrValue );
    }

    /// Write the definition of this style to @p writer, using the OASIS format.
    /// @param elementName the name of the XML element, e.g. "style:style"
    /// @param name must come from the collection
    void writeStyle( KoXmlWriter* writer, const char* elementName, const QString& name );

    /// QMap requires a complete sorting order.
    /// Another solution would have been a qdict and a key() here, a la KoTextFormat,
    /// but the key was difficult to generate.
    /// Solutions with only a hash value (not representative of the whole data)
    /// require us to write a hashtable by hand....
    bool operator<( const KoGenStyle &other ) const {
        if ( m_parentName != other.m_parentName ) return m_parentName < other.m_parentName;
        if ( m_properties.count() != other.m_properties.count() ) return m_properties.count() < other.m_properties.count();
        if ( m_attributes.count() != other.m_attributes.count() ) return m_attributes.count() < other.m_attributes.count();
        // Same number of properties and attributes, no other choice than iterating
        QMap<QString, QString>::const_iterator it = m_properties.begin();
        QMap<QString, QString>::const_iterator oit = other.m_properties.begin();
        for ( ; it != m_properties.end(); ++it, ++oit ) {
            if ( it.key() != oit.key() )
                return it.key() < oit.key();
            if ( it.data() != oit.data() )
                return it.data() < oit.data();
        }
        it = m_attributes.begin();
        oit = other.m_attributes.begin();
        for ( ; it != m_attributes.end(); ++it, ++oit ) {
            if ( it.key() != oit.key() )
                return it.key() < oit.key();
            if ( it.data() != oit.data() )
                return it.data() < oit.data();
        }
        return false;
    }

    /// Not needed for QMap, but can still be useful
    bool operator==( const KoGenStyle &other ) const {
        if ( m_parentName != other.m_parentName ) return false;
        if ( m_properties.count() != other.m_properties.count() ) return false;
        if ( m_attributes.count() != other.m_attributes.count() ) return false;
        // Same number of properties and attributes, no other choice than iterating
        QMap<QString, QString>::const_iterator it = m_properties.begin();
        QMap<QString, QString>::const_iterator oit = other.m_properties.begin();
        for ( ; it != m_properties.end(); ++it, ++oit ) {
            if ( it.key() != oit.key() || it.data() != oit.data() )
                return false;
        }
        it = m_attributes.begin();
        oit = other.m_attributes.begin();
        for ( ; it != m_attributes.end(); ++it, ++oit ) {
            if ( it.key() != oit.key() || it.data() != oit.data() )
                return false;
        }
        return true;
    }

    // Note that the copy constructor and assignment operator are allowed.
private:
    QString m_parentName;
    /// We use QMaps since they provide automatic sorting on the key (important for unicity!)
    QMap<QString, QString> m_properties;
    QMap<QString, QString> m_attributes;
};

#endif /* KOGENSTYLES_H */
