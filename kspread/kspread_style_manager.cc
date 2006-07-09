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
#include <qstringlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <KoOasisStyles.h>
#include <KoXmlNS.h>

#include "kspread_doc.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"

using namespace KSpread;

StyleManager::StyleManager()
  : m_defaultStyle( new CustomStyle() )
{
}

StyleManager::~StyleManager()
{
  delete m_defaultStyle;

  Styles::iterator iter = m_styles.begin();
  Styles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    delete iter.data();

    ++iter;
  }
}

void StyleManager::saveOasis( KoGenStyles &mainStyles )
{
    kdDebug() << "Saving default oasis style" << endl;
    KoGenStyle defaultStyle = KoGenStyle( Doc::STYLE_CELL_USER, "table-cell" );
    m_defaultStyle->saveOasis( defaultStyle, mainStyles );

    Styles::iterator iter = m_styles.begin();
    Styles::iterator end  = m_styles.end();

    while ( iter != end )
    {
        kdDebug() << "Saving style" << endl;
        CustomStyle * styleData = iter.data();

        KoGenStyle customStyle = KoGenStyle( Doc::STYLE_CELL_USER, "table-cell" );
        styleData->saveOasis( customStyle, mainStyles );

        ++iter;
    }
}

void StyleManager::loadOasisStyleTemplate( KoOasisStyles& oasisStyles )
{
    // loading default style first
    const QDomElement* defaultStyle = oasisStyles.defaultStyle( "table-cell" );
    if ( defaultStyle )
    {
      m_defaultStyle->loadOasis( oasisStyles, *defaultStyle, "Default" );
      m_defaultStyle->setType( Style::BUILTIN );
      kdDebug() << "StyleManager: default cell style loaded!" << endl;
    }
    else
    {
      delete m_defaultStyle;
      m_defaultStyle = new CustomStyle();
    }

    uint nStyles = oasisStyles.userStyles().count();
    kdDebug() << " number of template style to load : " << nStyles << endl;
    for (unsigned int item = 0; item < nStyles; item++) {
        QDomElement styleElem = oasisStyles.userStyles()[item];

        // assume the name assigned by the application
        QString name = styleElem.attributeNS( KoXmlNS::style, "name", QString::null );

        // then replace by user-visible one (if any)
        name = styleElem.attributeNS( KoXmlNS::style, "display-name", name );

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
            style->setType( Style::CUSTOM );
            m_styles[name] = style;
            kdDebug() << "Style " << name << ": " << style << endl;
        }
    }

    // set the parent pointers after we loaded all styles
    Styles::iterator iter = m_styles.begin();
    Styles::iterator end  = m_styles.end();
    while ( iter != end )
    {
        CustomStyle * styleData = iter.data();

        if ( !styleData->parent() && !styleData->parentName().isNull() )
            styleData->setParent( m_styles[ styleData->parentName() ] );

        ++iter;
    }
}

QDomElement StyleManager::save( QDomDocument & doc )
{
  kdDebug() << "Saving styles" << endl;
  QDomElement styles = doc.createElement( "styles" );

  kdDebug() << "Saving default style" << endl;
  m_defaultStyle->save( doc, styles );

  Styles::iterator iter = m_styles.begin();
  Styles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    kdDebug() << "Saving style" << endl;
    CustomStyle * styleData = iter.data();

    styleData->save( doc, styles );

    ++iter;
  }

  kdDebug() << "End saving styles" << endl;
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
      m_styles[name] = style;
      kdDebug() << "Style " << name << ": " << style << endl;
    }

    e = e.nextSibling().toElement();
  }

  Styles::iterator iter = m_styles.begin();
  Styles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    CustomStyle * styleData = iter.data();

    if ( !styleData->parent() && !styleData->parentName().isNull() )
      styleData->setParent( m_styles[ styleData->parentName() ] );

    ++iter;
  }

  m_defaultStyle->setName( "Default" );
  m_defaultStyle->setType( Style::BUILTIN );

  return true;
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
  Styles::const_iterator iter( m_styles.find( name ) );

  if ( iter != m_styles.end() )
    return iter.data();

  if ( name == "Default" )
    return m_defaultStyle;

  return 0;
}

void StyleManager::takeStyle( CustomStyle * style )
{
  CustomStyle * parent = style->parent();

  Styles::iterator iter = m_styles.begin();
  Styles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.data()->parent() == style )
      iter.data()->setParent( parent );

    ++iter;
  }

  Styles::iterator i( m_styles.find( style->name() ) );

  if ( i != m_styles.end() )
  {
    kdDebug() << "Erasing style entry for " << style->name() << endl;
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

  Styles::const_iterator iter = m_styles.begin();
  Styles::const_iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.key() == name && iter.data() != style )
      return false;

    ++iter;
  }

  return true;
}

void StyleManager::changeName( QString const & oldName, QString const & newName )
{
  Styles::iterator iter = m_styles.begin();
  Styles::iterator end  = m_styles.end();

  while ( iter != end )
  {
    if ( iter.data()->parentName() == oldName )
      iter.data()->refreshParentName();

    ++iter;
  }

  iter = m_styles.find( oldName );
  if ( iter != end )
  {
    CustomStyle * s = iter.data();
    m_styles.erase( iter );
    m_styles[newName] = s;
  }
}

QStringList StyleManager::styleNames() const
{
  QStringList list;

  list.push_back( i18n("Default") );

  Styles::const_iterator iter = m_styles.begin();
  Styles::const_iterator end  = m_styles.end();

  while ( iter != end )
  {
    list.push_back( iter.key() );

    ++iter;
  }

  return list;
}

