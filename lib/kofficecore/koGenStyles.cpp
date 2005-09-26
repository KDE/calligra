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

#include "koGenStyles.h"
#include <koxmlwriter.h>
#include <float.h>
#include <kdebug.h>

KoGenStyles::KoGenStyles()
{
}

KoGenStyles::~KoGenStyles()
{
}

QString KoGenStyles::lookup( const KoGenStyle& style, const QString& name, bool forceNumbering )
{
    StyleMap::iterator it = m_styleMap.find( style );
    if ( it == m_styleMap.end() ) {
        // Not found, try if this style is in fact equal to its parent (the find above
        // wouldn't have found it, due to m_parentName being set).
        if ( !style.parentName().isEmpty() ) {
            KoGenStyle testStyle( style );
            const KoGenStyle* parentStyle = this->style( style.parentName() ); // ## linear search
            if( !parentStyle ) {
                kdDebug(30003) << "KoGenStyles::lookup(" << name << "): parent style '" << style.parentName() << "' not found in collection" << endl;
            } else {
                testStyle.m_parentName = parentStyle->m_parentName;
                if ( *parentStyle == testStyle )
                    return style.parentName();
            }
        }

        QString styleName( name );
        if ( styleName.isEmpty() ) {
            styleName = 'A'; // for "auto".
            forceNumbering = true;
        }
        styleName = makeUniqueName( styleName, forceNumbering );
        m_styleNames.insert( styleName, false );
        it = m_styleMap.insert( style, styleName );
        NamedStyle s;
        s.style = &it.key();
        s.name = styleName;
        m_styleArray.append( s );
    }
    return it.data();
}

QString KoGenStyles::makeUniqueName( const QString& base, bool forceNumbering ) const
{
    if ( !forceNumbering && m_styleNames.find( base ) == m_styleNames.end() )
        return base;
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number( num++ );
    } while ( m_styleNames.find( name ) != m_styleNames.end() );
    return name;
}


void KoGenStyles::markStyleForStylesXml( const QString& name )
{
    NameMap::iterator it = m_styleNames.find( name );
    if ( it == m_styleNames.end() ) {
        kdWarning(30003) << "Style to be marked was not found:" << name << endl;
        return;
    }
    it.data() = true;
}

QValueList<KoGenStyles::NamedStyle> KoGenStyles::styles( int type, bool markedForStylesXml ) const
{
    QValueList<KoGenStyles::NamedStyle> lst;
    StyleArray::const_iterator it = m_styleArray.begin();
    StyleArray::const_iterator end = m_styleArray.end();
    for ( ; it != end ; ++it ) {
        if ( (*it).style->type() == type && m_styleNames[(*it).name] == markedForStylesXml ) {
            //NamedStyle s;
            //s.style = (*it).style;
            //s.name = (*it).name;
            lst.append( *it );
        }
    }
    return lst;
}

const KoGenStyle* KoGenStyles::style( const QString& name ) const
{
    StyleArray::const_iterator it = m_styleArray.begin();
    const StyleArray::const_iterator end = m_styleArray.end();
    for ( ; it != end ; ++it ) {
        if ( (*it).name == name )
            return (*it).style;
    }
    return 0;
}

KoGenStyle* KoGenStyles::styleForModification( const QString& name )
{
    return const_cast<KoGenStyle *>( style( name ) );
}

// Returns -1, 0 (equal) or 1
static int compareMap( const QMap<QString, QString>& map1, const QMap<QString, QString>& map2 )
{
    QMap<QString, QString>::const_iterator it = map1.begin();
    QMap<QString, QString>::const_iterator oit = map2.begin();
    for ( ; it != map1.end(); ++it, ++oit ) { // both maps have been checked for size already
        if ( it.key() != oit.key() )
            return it.key() < oit.key() ? -1 : +1;
        if ( it.data() != oit.data() )
            return it.data() < oit.data() ? -1 : +1;
    }
    return 0; // equal
}

////

void KoGenStyle::writeStyle( KoXmlWriter* writer, KoGenStyles& styles, const char* elementName, const QString& name, const char* propertiesElementName, bool closeElement, bool drawElement ) const
{
    //kdDebug(30003) << "writing out style " << name << " " << m_attributes["style:display-name"] << " " << m_familyName << endl;
    writer->startElement( elementName );
    if ( !drawElement )
        writer->addAttribute( "style:name", name );
    else
        writer->addAttribute( "draw:name", name );
    const KoGenStyle* parentStyle = 0;
    if ( !m_parentName.isEmpty() ) {
        writer->addAttribute( "style:parent-style-name", m_parentName );
        parentStyle = styles.style( m_parentName );
        if ( parentStyle && m_familyName.isEmpty() ) {
            // get family from parent style, just in case
            // Note: this is saving code, don't convert to attributeNS!
            const_cast<KoGenStyle *>( this )->
                m_familyName = parentStyle->attribute( "style:family" ).latin1();
        }
    }
    if ( !m_familyName.isEmpty() )
        const_cast<KoGenStyle *>( this )->
            addAttribute( "style:family", QString::fromLatin1( m_familyName ) );
    else {
        if ( qstrcmp( elementName, "style:style" ) == 0 )
            kdWarning(30003) << "User style " << name << " is without family - invalid. m_type=" << m_type << endl;
    }

#if 0 // #ifndef NDEBUG
    kdDebug() << "style: " << name << endl;
    printDebug();
    if ( parentStyle ) {
        kdDebug() << " parent: " << m_parentName << endl;
        parentStyle->printDebug();
    }
#endif

    // Write attributes [which differ from the parent style]
    // We only look at the direct parent style because we assume
    // that styles are fully specified, i.e. the inheritance is
    // only in the final file, not in the caller's code.
    QMap<QString, QString>::const_iterator it = m_attributes.begin();
    for ( ; it != m_attributes.end(); ++it ) {
        bool writeit = true;
        if ( parentStyle && it.key() != "style:family" // always write the family out
             && parentStyle->attribute( it.key() ) == it.data() )
            writeit = false;
        if ( writeit )
            writer->addAttribute( it.key().utf8(), it.data().utf8() );
    }
    bool createPropertiesTag = propertiesElementName && propertiesElementName[0] != '\0';
    KoGenStyle::PropertyType i = KoGenStyle::DefaultType;
    if ( !m_properties[i].isEmpty() ||
         !m_properties[KoGenStyle::ChildElement].isEmpty() ) {
        if ( createPropertiesTag )
            writer->startElement( propertiesElementName ); // e.g. paragraph-properties
        it = m_properties[i].begin();
        for ( ; it != m_properties[i].end(); ++it ) {
            if ( !parentStyle || parentStyle->property( it.key(), i ) != it.data() )
                writer->addAttribute( it.key().utf8(), it.data().utf8() );
        }
        i = KoGenStyle::ChildElement;
        it = m_properties[i].begin();
        for ( ; it != m_properties[i].end(); ++it ) {
            if ( !parentStyle || parentStyle->property( it.key(), i ) != it.data() ) {
                writer->addCompleteElement( it.data().utf8() );
            }
        }
        if ( createPropertiesTag )
            writer->endElement();
    }
    i = KoGenStyle::TextType;
    if ( !m_properties[i].isEmpty() ) {
        writer->startElement( "style:text-properties" );
        it = m_properties[i].begin();
        for ( ; it != m_properties[i].end(); ++it ) {
            if ( !parentStyle || parentStyle->property( it.key(), i ) != it.data() )
                writer->addAttribute( it.key().utf8(), it.data().utf8() );
        }
        writer->endElement();
    }
    // And now the style maps
    for ( uint i = 0; i < m_maps.count(); ++i ) {
        bool writeit = true;
        if ( parentStyle && compareMap( m_maps[i], parentStyle->m_maps[i] ) == 0 )
            writeit = false;
        if ( writeit ) {
            writer->startElement( "style:map" );
            QMap<QString, QString>::const_iterator it = m_maps[i].begin();
            for ( ; it != m_maps[i].end(); ++it ) {
                writer->addAttribute( it.key().utf8(), it.data().utf8() );
            }
            writer->endElement(); // style:map
        }
    }
    if ( closeElement )
        writer->endElement();
}

void KoGenStyle::addPropertyPt( const QString& propName, double propValue, PropertyType type )
{
    QString str;
    str.setNum( propValue, 'g', DBL_DIG );
    str += "pt";
    m_properties[type].insert( propName, str );
}

void KoGenStyle::addAttributePt( const QString& attrName, double attrValue )
{
    QString str;
    str.setNum( attrValue, 'g', DBL_DIG );
    str += "pt";
    m_attributes.insert( attrName, str );
}

#ifndef NDEBUG
void KoGenStyle::printDebug() const
{
    int i = DefaultType;
    kdDebug() << m_properties[i].count() << " properties." << endl;
    for( QMap<QString,QString>::ConstIterator it = m_properties[i].begin(); it != m_properties[i].end(); ++it ) {
        kdDebug() << "     " << it.key() << " = " << it.data() << endl;
    }
    i = TextType;
    kdDebug() << m_properties[i].count() << " text properties." << endl;
    for( QMap<QString,QString>::ConstIterator it = m_properties[i].begin(); it != m_properties[i].end(); ++it ) {
        kdDebug() << "     " << it.key() << " = " << it.data() << endl;
    }
    i = ChildElement;
    kdDebug() << m_properties[i].count() << " child elements." << endl;
    for( QMap<QString,QString>::ConstIterator it = m_properties[i].begin(); it != m_properties[i].end(); ++it ) {
        kdDebug() << "     " << it.key() << " = " << it.data() << endl;
    }
    kdDebug() << m_attributes.count() << " attributes." << endl;
    for( QMap<QString,QString>::ConstIterator it = m_attributes.begin(); it != m_attributes.end(); ++it ) {
        kdDebug() << "     " << it.key() << " = " << it.data() << endl;
    }
    kdDebug() << m_maps.count() << " maps." << endl;
    for ( uint i = 0; i < m_maps.count(); ++i ) {
        kdDebug() << "map " << i << ":" << endl;
        for( QMap<QString,QString>::ConstIterator it = m_maps[i].begin(); it != m_maps[i].end(); ++it ) {
            kdDebug() << "     " << it.key() << " = " << it.data() << endl;
        }
    }
    kdDebug() << endl;
}
#endif

bool KoGenStyle::operator<( const KoGenStyle &other ) const
{
    if ( m_type != other.m_type ) return m_type < other.m_type;
    if ( m_parentName != other.m_parentName ) return m_parentName < other.m_parentName;
    for ( uint i = 0 ; i < N_NumTypes ; ++i )
        if ( m_properties[i].count() != other.m_properties[i].count() )
            return m_properties[i].count() < other.m_properties[i].count();
    if ( m_attributes.count() != other.m_attributes.count() ) return m_attributes.count() < other.m_attributes.count();
    if ( m_maps.count() != other.m_maps.count() ) return m_maps.count() < other.m_maps.count();
    // Same number of properties and attributes, no other choice than iterating
    for ( uint i = 0 ; i < N_NumTypes ; ++i ) {
        int comp = compareMap( m_properties[i], other.m_properties[i] );
        if ( comp != 0 )
            return comp < 0;
    }
    int comp = compareMap( m_attributes, other.m_attributes );
    if ( comp != 0 )
        return comp < 0;
    for ( uint i = 0 ; i < m_maps.count() ; ++i ) {
        int comp = compareMap( m_maps[i], other.m_maps[i] );
        if ( comp != 0 )
            return comp < 0;
    }
    return false;
}

bool KoGenStyle::operator==( const KoGenStyle &other ) const
{
    if ( m_type != other.m_type ) return false;
    if ( m_parentName != other.m_parentName ) return false;
    for ( uint i = 0 ; i < N_NumTypes ; ++i )
        if ( m_properties[i].count() != other.m_properties[i].count() )
            return false;
    if ( m_attributes.count() != other.m_attributes.count() ) return false;
    if ( m_maps.count() != other.m_maps.count() ) return false;
    // Same number of properties and attributes, no other choice than iterating
    for ( uint i = 0 ; i < N_NumTypes ; ++i ) {
        int comp = compareMap( m_properties[i], other.m_properties[i] );
        if ( comp != 0 )
            return false;
    }
    int comp = compareMap( m_attributes, other.m_attributes );
    if ( comp != 0 )
        return false;
    for ( uint i = 0 ; i < m_maps.count() ; ++i ) {
        int comp = compareMap( m_maps[i], other.m_maps[i] );
        if ( comp != 0 )
            return false;
    }
    return true;
}
