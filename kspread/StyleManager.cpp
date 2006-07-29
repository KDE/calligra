/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <QStringList>

#include <kdebug.h>
#include <klocale.h>

#include <KoOasisStyles.h>
#include <KoXmlNS.h>

#include "Doc.h"
#include "Style.h"
#include "StyleManager.h"

using namespace KSpread;

StyleManager::StyleManager()
  : m_defaultStyle( new CustomStyle() )
{
}

StyleManager::~StyleManager()
{
  delete m_defaultStyle;
  qDeleteAll(m_styles);
}

void StyleManager::saveOasis( KoGenStyles &mainStyles )
{
    kDebug() << "StyleManager: Saving default cell style" << endl;
    KoGenStyle defStyle = KoGenStyle( Doc::STYLE_CELL_USER, "table-cell" );
    defaultStyle()->saveOasis( defStyle, mainStyles );

    QStringList names = styleNames ();
    QStringList::iterator it;
    for (it = names.begin(); it != names.end(); ++it) {
      if (*it != "Default") {
        kDebug() << "StyleManager: Saving common cell style " << *it <<endl;
        CustomStyle * styleData = style (*it);
        KoGenStyle customStyle = KoGenStyle( Doc::STYLE_CELL_USER, "table-cell" );
        styleData->saveOasis( customStyle, mainStyles );
      }
    }
}

void StyleManager::loadOasisStyleTemplate( KoOasisStyles& oasisStyles )
{
    // reset the map of OpenDocument Styles
    m_oasisStyles.clear();

    // loading default style first
    const QDomElement* defStyle = oasisStyles.defaultStyle( "table-cell" );
    if ( defStyle )
    {
      kDebug() << "StyleManager: Loading default cell style" << endl;
      defaultStyle()->loadOasis( oasisStyles, *defStyle, "Default" );
      defaultStyle()->setType( Style::BUILTIN );
    }
    else
      resetDefaultStyle();
    // insert it into the the map sorted the OpenDocument name
    m_oasisStyles["Default"] = defaultStyle();

    uint nStyles = oasisStyles.userStyles().count();
    for (unsigned int item = 0; item < nStyles; item++) {
        QDomElement styleElem = oasisStyles.userStyles()[item];

        // assume the name assigned by the application
        const QString oasisName = styleElem.attributeNS( KoXmlNS::style, "name", QString::null );

        // then replace by user-visible one (if any)
        const QString name = styleElem.attributeNS( KoXmlNS::style, "display-name", oasisName );
        kDebug() << " StyleManager: Loading common cell style: " << oasisName << " (display name: " << name << ")" << endl;

        if ( !name.isEmpty() )
        {
            CustomStyle * style = 0;
            if ( styleElem.hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
                // The style's parent name will be set in Style::loadOasis(..).
                // After all styles are loaded the pointer to the parent is set.
                style = new CustomStyle( name, 0 );
            else
                style = new CustomStyle( name, m_defaultStyle );

            //fixme test return;
            style->loadOasis( oasisStyles, styleElem, name );
            insertStyle (style);
            // insert it into the the map sorted the OpenDocument name
            m_oasisStyles[oasisName] = style;
            kDebug() << "Style " << name << ": " << style << endl;
        }
    }

    // reparent all styles
    foreach ( CustomStyle* style, m_styles )
      if ( style->name() != "Default" )
        if ( !style->parent() && !style->parentName().isNull() )
          style->setParent( m_oasisStyles[ style->parentName() ] );
}

QDomElement StyleManager::save( QDomDocument & doc )
{
  QDomElement styles = doc.createElement( "styles" );

  m_defaultStyle->save( doc, styles );

  CustomStyles::iterator iter = m_styles.begin();
  CustomStyles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    CustomStyle * styleData = iter.value();

    styleData->save( doc, styles );

    ++iter;
  }

  return styles;
}

bool StyleManager::loadXML( QDomElement const & styles )
{
  QDomElement e = styles.firstChild().toElement();
  while ( !e.isNull() )
  {
    QString name;
    if ( e.hasAttribute( "name" ) )
      name = e.attribute( "name" );

    if ( name == "Default" )
    {
      if ( !m_defaultStyle->loadXML( e, name ) )
        return false;
      m_defaultStyle->setType( Style::BUILTIN );
    }
    else if ( !name.isNull() )
    {
      CustomStyle * style = 0;
      if ( e.hasAttribute( "parent" ) && e.attribute( "parent" ) == "Default" )
        style = new CustomStyle( name, m_defaultStyle );
      else
        style = new CustomStyle( name, 0 );

      if ( !style->loadXML( e, name ) )
      {
        delete style;
        return false;
      }

      if ( style->type() == Style::AUTO )
        style->setType( Style::CUSTOM );
      insertStyle (style);
      kDebug() << "Style " << name << ": " << style << endl;
    }

    e = e.nextSibling().toElement();
  }

  defaultStyle()->setName( "Default" );
  defaultStyle()->setType( Style::BUILTIN );
  
  // reparent all styles
  QStringList names = styleNames ();
  QStringList::iterator it;
  for (it = names.begin(); it != names.end(); ++it) {
    if (*it != "Default") {
      CustomStyle * styleData = style (*it);
      if ( !styleData->parent() && !styleData->parentName().isNull() )
        styleData->setParent( m_styles[ styleData->parentName() ] );
    }
  }

  return true;
}

void StyleManager::resetDefaultStyle ()
{
  delete m_defaultStyle;
  m_defaultStyle = new CustomStyle;
}

void StyleManager::createBuiltinStyles()
{
  CustomStyle * header1 = new CustomStyle( i18n( "Header" ), m_defaultStyle );
  QFont f( header1->font() );
  f.setItalic( true );
  f.setPointSize( f.pointSize() + 2 );
  f.setBold( true );
  header1->changeFont( f );
  header1->setType( Style::BUILTIN );
  m_styles[ header1->name() ] = header1;

  CustomStyle * header2 = new CustomStyle( i18n( "Header1" ), header1 );
  QColor color( "#F0F0FF" );
  header2->changeBgColor( color );
  QPen pen( Qt::black, 1, Qt::SolidLine );
  header2->changeBottomBorderPen( pen );
  header2->setType( Style::BUILTIN );

  m_styles[ header2->name() ] = header2;
}

CustomStyle * StyleManager::style( QString const & name ) const
{
  if (m_styles.contains(name))
    return m_styles[name];

  if ( name == "Default" )
    return m_defaultStyle;

  return 0;
}

void StyleManager::takeStyle( CustomStyle * style )
{
  CustomStyle * parent = style->parent();

  CustomStyles::iterator iter = m_styles.begin();
  CustomStyles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.value()->parent() == style )
      iter.value()->setParent( parent );

    ++iter;
  }

  CustomStyles::iterator i( m_styles.find( style->name() ) );

  if ( i != m_styles.end() )
  {
    kDebug() << "Erasing style entry for " << style->name() << endl;
    m_styles.erase( i );
  }
}

bool StyleManager::checkCircle( QString const & name, QString const & parent )
{
  CustomStyle * s = style( parent );
  if ( !s || s->parent() == 0 )
    return true;
  if ( s->parentName() == name )
    return false;
  else
    return checkCircle( name, s->parentName() );
}

bool StyleManager::validateStyleName( QString const & name, CustomStyle * style )
{
  if ( m_defaultStyle->name() == name || name == "Default" )
    return false;

  CustomStyles::const_iterator iter = m_styles.begin();
  CustomStyles::const_iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.key() == name && iter.value() != style )
      return false;

    ++iter;
  }

  return true;
}

void StyleManager::changeName( QString const & oldName, QString const & newName )
{
  CustomStyles::iterator iter = m_styles.begin();
  CustomStyles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.value()->parentName() == oldName )
      iter.value()->refreshParentName();

    ++iter;
  }

  iter = m_styles.find( oldName );
  if ( iter != end )
  {
    CustomStyle * s = iter.value();
    m_styles.erase( iter );
    m_styles[newName] = s;
  }
}

void StyleManager::insertStyle (CustomStyle *style)
{
  QString name = style->name();
  if (m_styles.contains(name) && (m_styles[name] != style))
    delete m_styles[name];
  m_styles[name] = style;
}

QStringList StyleManager::styleNames() const
{
  QStringList list;

  list.push_back( i18n("Default") );

  CustomStyles::const_iterator iter = m_styles.begin();
  CustomStyles::const_iterator end  = m_styles.end();

  while ( iter != end )
  {
    list.push_back( iter.key() );

    ++iter;
  }

  return list;
}

Styles StyleManager::loadOasisAutoStyles( KoOasisStyles& oasisStyles )
{
  Q3DictIterator<QDomElement> it( oasisStyles.styles("table-cell") );
  Styles autoStyles;
  for (;it.current();++it)
  {
    if ( it.current()->hasAttributeNS( KoXmlNS::style , "name" ) )
    {
      QString name = it.current()->attributeNS( KoXmlNS::style , "name" , QString::null );
      kDebug() << "StyleManager: Preloading automatic cell style: " << name << endl;
      autoStyles.insert( name , new Style());
      autoStyles[name]->loadOasisStyle( oasisStyles , *(it.current()) );

      if ( it.current()->hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
      {
        QString parentStyleName = it.current()->attributeNS( KoXmlNS::style, "parent-style-name", QString::null );
        if ( m_oasisStyles.contains( parentStyleName ) )
        {
          autoStyles[name]->setParent( m_oasisStyles[parentStyleName] );
        }
        kDebug() << "\t parent-style-name:" << parentStyleName << endl;
      }
      else
      {
        autoStyles[name]->setParent( m_defaultStyle );
      }
    }
  }
  return autoStyles;
}

void StyleManager::releaseUnusedAutoStyles( Styles autoStyles )
{
  foreach ( Style* style, autoStyles.values() )
  {
    if ( style->release() )
      delete style;
  }

  // Now, we can clear the map of styles sorted by OpenDocument name.
  m_oasisStyles.clear();
}
