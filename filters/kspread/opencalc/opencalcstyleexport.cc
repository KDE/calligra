/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <opencalcstyleexport.h>

#include <qdom.h>

OpenCalcStyles::OpenCalcStyles()
{
  m_cellStyles.setAutoDelete( true );
  m_columnStyles.setAutoDelete( true );
  m_numberStyles.setAutoDelete( true );
  m_rowStyles.setAutoDelete( true );
  m_tableStyles.setAutoDelete( true );
}

OpenCalcStyles::~OpenCalcStyles()
{
}

void OpenCalcStyles::writeStyles( QDomDocument & doc, QDomElement & autoStyles )
{
  addColumnStyles( doc, autoStyles );
  addRowStyles( doc, autoStyles );
  addTableStyles( doc, autoStyles );
  addNumberStyles( doc, autoStyles );
  addCellStyles( doc, autoStyles );
}

QString OpenCalcStyles::cellStyle( CellStyle const & cs )
{
}

QString OpenCalcStyles::columnStyle( ColumnStyle const & cs )
{
  ColumnStyle * t = m_columnStyles.first();
  while ( t )
  {
    if ( ColumnStyle::isEqual( t, cs ) )
      return t->name;

    t = m_columnStyles.next();
  }

  t = new ColumnStyle();
  t->copyData( cs );

  m_columnStyles.append( t );

  t->name = QString( "co%1" ).arg( m_columnStyles.count() );

  return t->name;
}

QString OpenCalcStyles::numberStyle( NumberStyle const & ns )
{
}

QString OpenCalcStyles::rowStyle( RowStyle const & rs )
{
  RowStyle * t = m_rowStyles.first();
  while ( t )
  {
    if ( RowStyle::isEqual( t, rs ) )
      return t->name;

    t = m_rowStyles.next();
  }

  t = new RowStyle();
  t->copyData( rs );

  m_rowStyles.append( t );

  t->name = QString( "ro%1" ).arg( m_rowStyles.count() );

  return t->name;
}

QString OpenCalcStyles::tableStyle( TableStyle const & ts )
{
  TableStyle * t = m_tableStyles.first();
  while ( t )
  {
    if ( TableStyle::isEqual( t, ts ) )
      return t->name;

    t = m_tableStyles.next();
  }

  t = new TableStyle();
  t->copyData( ts );

  m_tableStyles.append( t );

  t->name = QString( "ta%1" ).arg( m_tableStyles.count() );

  return t->name;
}

void OpenCalcStyles::addCellStyles( QDomDocument & doc, QDomElement & autoStyles )
{
}

void OpenCalcStyles::addColumnStyles( QDomDocument & doc, QDomElement & autoStyles )
{
  ColumnStyle * t = m_columnStyles.first();
  while ( t )
  {
    QDomElement ts = doc.createElement( "style:style" );
    ts.setAttribute( "style:name", t->name );
    ts.setAttribute( "style:family", "table-column" );

    QDomElement prop = doc.createElement( "style:properties" );
    if ( t->breakB != Style::none )
      prop.setAttribute( "fo:break-before", ( t->breakB == Style::automatic ? "auto" : "page" ) );
    prop.setAttribute( "style:column-width", QString( "%1cm" ).arg( t->size ) );

    ts.appendChild( prop );
    autoStyles.appendChild( ts );
    
    t = m_columnStyles.next();
  }  
}

void OpenCalcStyles::addNumberStyles( QDomDocument & doc, QDomElement & autoStyles )
{
}

void OpenCalcStyles::addRowStyles( QDomDocument & doc, QDomElement & autoStyles )
{
  RowStyle * t = m_rowStyles.first();
  while ( t )
  {
    QDomElement ts = doc.createElement( "style:style" );
    ts.setAttribute( "style:name", t->name );
    ts.setAttribute( "style:family", "table-row" );

    QDomElement prop = doc.createElement( "style:properties" );
    prop.setAttribute( "style:row-height", QString( "%1cm" ).arg( t->size ) );
    if ( t->breakB != Style::none )
      prop.setAttribute( "fo:break-before", ( t->breakB == Style::automatic ? "auto" : "page" ) );

    ts.appendChild( prop );
    autoStyles.appendChild( ts );
    
    t = m_rowStyles.next();
  }  
}

void OpenCalcStyles::addTableStyles( QDomDocument & doc, QDomElement & autoStyles )
{
  TableStyle * t = m_tableStyles.first();
  while ( t )
  {
    QDomElement ts = doc.createElement( "style:style" );
    ts.setAttribute( "style:name", t->name );
    ts.setAttribute( "style:family", "table" );
    ts.setAttribute( "style:master-page-name", "Default" );

    QDomElement prop = doc.createElement( "style:properties" );
    prop.setAttribute( "table:display", ( t->visible ? "true" : "false" ) );

    ts.appendChild( prop );
    autoStyles.appendChild( ts );
    
    t = m_tableStyles.next();
  }  
}

bool TableStyle::isEqual( TableStyle const * const t1, TableStyle const & t2 )
{
  if ( t1->visible == t2.visible )
    return true;

  return false;
}

void ColumnStyle::copyData( ColumnStyle const & cs )
{
  breakB = cs.breakB;
  size   = cs.size;
}

bool ColumnStyle::isEqual( ColumnStyle const * const c1, ColumnStyle const & c2 )
{
  if ( ( c1->breakB == c2.breakB ) && ( c1->size == c2.size ) )
    return true;

  return false;
}

void RowStyle::copyData( RowStyle const & cs )
{
  breakB = cs.breakB;
  size   = cs.size;
}

bool RowStyle::isEqual( RowStyle const * const c1, RowStyle const & c2 )
{
  if ( ( c1->breakB == c2.breakB ) && ( c1->size == c2.size ) )
    return true;

  return false;
}
