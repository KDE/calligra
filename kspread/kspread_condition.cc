/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 - 2003 The KSpread Team
                             www.koffice.org/kspread

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

#include <float.h>

#include "kspread_cell.h"
#include "kspread_condition.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_util.h"

#include <koxmlwriter.h>
#include <koxmlns.h>
#include <kdebug.h>
#include <qdom.h>
#include <qbuffer.h>

KSpreadConditional::KSpreadConditional():
  val1( 0.0 ), val2( 0.0 ), strVal1( 0 ), strVal2( 0 ),
  colorcond( 0 ), fontcond( 0 ), styleName( 0 ),
  style( 0 ), cond( None )
{
}

KSpreadConditional::~KSpreadConditional()
{
  delete strVal1;
  delete strVal2;
  delete colorcond;
  delete fontcond;
  delete styleName;
}

KSpreadConditional::KSpreadConditional( const KSpreadConditional& c )
{
  operator=( c );
}

KSpreadConditional& KSpreadConditional::operator=( const KSpreadConditional& d )
{
  strVal1 = d.strVal1 ? new QString( *d.strVal1 ) : 0;
  strVal2 = d.strVal2 ? new QString( *d.strVal2 ) : 0;
  styleName = d.styleName ? new QString( *d.styleName ) : 0;
  fontcond = d.fontcond ? new QFont( *d.fontcond ) : 0;
  colorcond = d.colorcond ? new QColor( *d.colorcond ) : 0;
  val1  = d.val1;
  val2  = d.val2;
  style = d.style;
  cond  = d.cond;

  return *this;
}


KSpreadConditions::KSpreadConditions( const KSpreadCell * ownerCell )
  : m_cell( ownerCell ), m_matchedStyle( 0 )
{
  Q_ASSERT( ownerCell != NULL );
}

KSpreadConditions::~KSpreadConditions()
{
  m_condList.clear();
}

void KSpreadConditions::checkMatches()
{
  KSpreadConditional condition;

  if ( currentCondition( condition ) )
    m_matchedStyle = condition.style;
  else
    m_matchedStyle = 0;
}

bool KSpreadConditions::currentCondition( KSpreadConditional & condition )
{
  /* for now, the first condition that is true is the one that will be used */

  QValueList<KSpreadConditional>::const_iterator it;
  double value   = m_cell->value().asFloat() * m_cell->factor( m_cell->column(), m_cell->row() );
  QString strVal = m_cell->text();

  //  if ( m_cell->value().isNumber() && !m_cell->table()->getShowFormula())
  //  {

  for ( it = m_condList.begin(); it != m_condList.end(); ++it )
  {
    condition = *it;
    kdDebug()<<"*it :"<< *( ( *it ).styleName )<<endl;
    kdDebug()<<"*it style :"<<(  *it ).style <<endl;

    if ( condition.strVal1 && m_cell->value().isNumber() )
      continue;

    switch ( condition.cond )
    {
     case Equal:
      if ( condition.strVal1 )
      {
        if ( strVal == *condition.strVal1 )
          return true;
      }
      else
      if ( value - condition.val1 < DBL_EPSILON &&
           value - condition.val1 > (0.0 - DBL_EPSILON) )
      {
        return true;
      }
      break;

     case Superior:
      if ( condition.strVal1 )
      {
        if ( strVal > *condition.strVal1 )
          return true;
      }
      else
      if ( value > condition.val1 )
      {
        return true;
      }
      break;

     case Inferior:
      if ( condition.strVal1 )
      {
        if ( strVal < *condition.strVal1 )
          return true;
      }
      else
      if ( value < condition.val1 )
      {
        return true;
      }
      break;

     case SuperiorEqual :
      if ( condition.strVal1 )
      {
        if ( strVal >= *condition.strVal1 )
          return true;
      }
      else
      if ( value >= condition.val1 )
      {
        return true;
      }
      break;

     case InferiorEqual :
      if ( condition.strVal1 )
      {
        if ( strVal <= *condition.strVal1 )
          return true;
      }
      else
      if ( value <= condition.val1 )
      {
        return true;
      }
      break;

     case Between :
      if ( condition.strVal1 && condition.strVal2 )
      {
        if ( strVal > *condition.strVal1 && strVal < *condition.strVal2 )
          return true;
      }
      else
      if ( ( value > QMIN(condition.val1, condition.val2 ) )
           && ( value < QMAX(condition.val1, condition.val2 ) ) )
      {
        return true;
      }
      break;

     case Different :
      if ( condition.strVal1 && condition.strVal2 )
      {
        if ( strVal < *condition.strVal1 || strVal > *condition.strVal2 )
          return true;
      }
      else
      if ( ( value < QMIN(condition.val1, condition.val2 ) )
           || ( value > QMAX(condition.val1, condition.val2) ) )
      {
        return true;
      }
      break;

     default:
      break;
    }
  }
  return false;
}

QValueList<KSpreadConditional> KSpreadConditions::conditionList() const
{
  return m_condList;
}

void KSpreadConditions::setConditionList( const QValueList<KSpreadConditional> & list )
{
  m_condList.clear();

  QValueList<KSpreadConditional>::const_iterator it;
  for ( it = list.begin(); it != list.end(); ++it )
  {
    KSpreadConditional d = *it;
    m_condList.append( KSpreadConditional( d ) );
  }
}

void KSpreadConditions::saveOasisConditions( KoGenStyle &currentCellStyle )
{
    //todo fix me with kspread old format!!!
    if ( m_condList.isEmpty() )
        return;
    QValueList<KSpreadConditional>::const_iterator it;
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level

    for ( it = m_condList.begin(); it != m_condList.end(); ++it )
    {
        KSpreadConditional condition = *it;
        elementWriter.startElement( "style:map" );
        elementWriter.addAttribute( "style:condition", saveOasisConditionValue( condition ) );
        elementWriter.addAttribute("style:apply-style-name", *( condition.styleName ) );
        //elementWriter.addAttribute( "style:base-cell-address", "..." );//todo
        elementWriter.endElement();
    }
    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    currentCellStyle.addChildElement( "conditional attribute", elementContents );

}

QString KSpreadConditions::saveOasisConditionValue( KSpreadConditional &condition)
{
    //we can also compare text value.
    //todo adapt it.
    QString value;
    switch( condition.cond )
    {
    case None:
        break;
    case Equal:
        value="cell-content()=";
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case Superior:
        value="cell-content()>";
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case Inferior:
        value="cell-content()<";
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case SuperiorEqual:
        value="cell-content()>=";
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case InferiorEqual:
        value="cell-content()<=";
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case Between:
        value="cell-content-is-between(";
        if ( condition.strVal1 )
        {
            value+=*condition.strVal1;
            value+=",";
            if ( condition.strVal2 )
                value+=*condition.strVal2;
        }
        else
        {
            value+=QString::number( condition.val1 );
            value+=",";
            value+=QString::number( condition.val2 );
        }
        value+=")";
        break;
    case DifferentTo:
        value="cell-content()!="; //FIXME not good here !
        if ( condition.strVal1 )
            value+=*condition.strVal1;
        else
            value+=QString::number( condition.val1 );
        break;
    case Different:
        value="cell-content-is-not-between(";
        if ( condition.strVal1 )
        {
            value+=*condition.strVal1;
            value+=",";
            if ( condition.strVal2 )
                value+=*condition.strVal2;
        }
        else
        {
            value+=QString::number( condition.val1 );
            value+=",";
            value+=QString::number( condition.val2 );
        }
        value+=")";
        break;
    }
    return value;
}


QDomElement KSpreadConditions::saveConditions( QDomDocument & doc ) const
{
  QDomElement conditions = doc.createElement("condition");
  QValueList<KSpreadConditional>::const_iterator it;
  QDomElement child;
  int num = 0;
  QString name;

  for ( it = m_condList.begin(); it != m_condList.end(); ++it )
  {
    KSpreadConditional condition = *it;

    /* the name of the element will be "condition<n>"
     * This is unimportant now but in older versions three conditions were
     * hardcoded with names "first" "second" and "third"
     */
    name.setNum( num );
    name.prepend( "condition" );

    child = doc.createElement( name );
    child.setAttribute( "cond", (int) condition.cond );

    // TODO: saving in KSpread 1.1 | KSpread 1.2 format
    if ( condition.strVal1 )
    {
      child.setAttribute( "strval1", *condition.strVal1 );
      if ( condition.strVal2 )
        child.setAttribute( "strval2", *condition.strVal2 );
    }
    else
    {
      child.setAttribute( "val1", condition.val1 );
      child.setAttribute( "val2", condition.val2 );
    }
    if ( condition.styleName )
    {
      child.setAttribute( "style", *condition.styleName );
    }
    else
    {
      child.setAttribute( "color", condition.colorcond->name() );
      child.appendChild( util_createElement( "font", *condition.fontcond, doc ) );
    }

    conditions.appendChild( child );

    ++num;
  }

  if ( num == 0 )
  {
    /* there weren't any real conditions -- return a null dom element */
    return QDomElement();
  }
  else
  {
    return conditions;
  }
}

void KSpreadConditions::loadOasisConditions( const QDomElement & element )
{
    kdDebug()<<"void KSpreadConditions::loadOasisConditions( const QDomElement & element )\n";
    QDomElement elementItem( element );
    KSpreadStyleManager * manager = m_cell->sheet()->doc()->styleManager();

    while ( !elementItem.isNull() )
    {
        kdDebug()<<"elementItem.tagName() :"<<elementItem.tagName()<<endl;
        if ( elementItem.tagName()== "style:map"  )
        {
            bool ok = true;
            kdDebug()<<"elementItem.attribute(style:condition ) :"<<elementItem.attributeNS( KoXmlNS::style, "condition", QString::null )<<endl;
            KSpreadConditional newCondition;
            loadOasisConditionValue( elementItem.attributeNS( KoXmlNS::style, "condition", QString::null ), newCondition );
            if ( elementItem.hasAttributeNS( KoXmlNS::style, "apply-style-name" ) )
            {
                kdDebug()<<"elementItem.attribute( style:apply-style-name ) :"<<elementItem.attributeNS( KoXmlNS::style, "apply-style-name", QString::null )<<endl;
                newCondition.styleName = new QString( elementItem.attributeNS( KoXmlNS::style, "apply-style-name", QString::null ) );
                newCondition.style = manager->style( *newCondition.styleName );
                if ( !newCondition.style )
                    ok = false;
                else
                    ok = true;
            }

            if ( ok )
                m_condList.append( newCondition );
            else
                kdDebug(36001) << "Error loading condition " << elementItem.nodeName()<< endl;
        }
        elementItem = elementItem.nextSibling().toElement();
    }
}

void KSpreadConditions::loadOasisConditionValue( const QString &styleCondition, KSpreadConditional &newCondition )
{
    QString val( styleCondition );
    if ( val.contains( "cell-content()" ) )
    {
        val = val.remove( "cell-content()" );
        loadOasisCondition( val,newCondition );
    }
    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if ( val.contains( "cell-content-is-between(" ) )
    {
        val = val.remove( "cell-content-is-between(" );
        val = val.remove( ")" );
        QStringList listVal = QStringList::split( "," , val );
        loadOasisValidationValue( listVal, newCondition );
        newCondition.cond = Between;
    }
    if ( val.contains( "cell-content-is-not-between(" ) )
    {
        val = val.remove( "cell-content-is-not-between(" );
        val = val.remove( ")" );
        QStringList listVal = QStringList::split( ",", val );
        loadOasisValidationValue( listVal,newCondition );
        newCondition.cond = Different;
    }

}


void KSpreadConditions::loadOasisCondition( QString &valExpression, KSpreadConditional &newCondition )
{
    QString value;
    if (valExpression.find( "<=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        newCondition.cond = InferiorEqual;
    }
    else if (valExpression.find( ">=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        newCondition.cond = SuperiorEqual;
    }
    else if (valExpression.find( "!=" )==0 )
    {
        //add Differentto attribute
        value = valExpression.remove( 0,2 );
        newCondition.cond = DifferentTo;
    }
    else if ( valExpression.find( "<" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Inferior;
    }
    else if(valExpression.find( ">" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Superior;
    }
    else if (valExpression.find( "=" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Equal;
    }
    else
        kdDebug()<<" I don't know how to parse it :"<<valExpression<<endl;
    kdDebug()<<" value :"<<value<<endl;
    bool ok = false;
    newCondition.val1 = value.toDouble(&ok);
    if ( !ok )
    {
        newCondition.val1 = value.toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal1 = new QString( value );
            kdDebug()<<" Try to parse this value :"<<value<<endl;
        }

    }
}


void KSpreadConditions::loadOasisValidationValue( const QStringList &listVal, KSpreadConditional &newCondition )
{
    bool ok = false;
    kdDebug()<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;

    newCondition.val1 = listVal[0].toDouble(&ok);
    if ( !ok )
    {
        newCondition.val1 = listVal[0].toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal1 = new QString( listVal[0] );
            kdDebug()<<" Try to parse this value :"<<listVal[0]<<endl;
        }
    }
    ok=false;
    newCondition.val2 = listVal[1].toDouble(&ok);
    if ( !ok )
    {
        newCondition.val2 = listVal[1].toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal2 = new QString( listVal[1] );
            kdDebug()<<" Try to parse this value :"<<listVal[1]<<endl;
        }
    }
}


void KSpreadConditions::loadConditions( const QDomElement & element )
{
  QDomNodeList nodeList = element.childNodes();
  KSpreadConditional newCondition;
  bool ok;
  KSpreadStyleManager * manager = m_cell->sheet()->doc()->styleManager();

  for ( int i = 0; i < (int)(nodeList.length()); ++i )
  {
    newCondition.strVal1   = 0;
    newCondition.strVal2   = 0;
    newCondition.styleName = 0;
    newCondition.fontcond  = 0;
    newCondition.colorcond = 0;

    QDomElement conditionElement = nodeList.item( i ).toElement();

    ok = conditionElement.hasAttribute( "cond" );

    if ( ok )
      newCondition.cond = (Conditional) conditionElement.attribute( "cond" ).toInt( &ok );
    else continue;

    if ( conditionElement.hasAttribute( "val1" ) )
    {
      newCondition.val1 = conditionElement.attribute( "val1" ).toDouble( &ok );

      if ( conditionElement.hasAttribute( "val2" ) )
        newCondition.val2 = conditionElement.attribute("val2").toDouble( &ok );
    }

    if ( conditionElement.hasAttribute( "strval1" ) )
    {
      newCondition.strVal1 = new QString( conditionElement.attribute( "strval1" ) );

      if ( conditionElement.hasAttribute( "strval2" ) )
        newCondition.strVal2 = new QString( conditionElement.attribute( "strval2" ) );
    }

    if ( conditionElement.hasAttribute( "color" ) )
      newCondition.colorcond = new QColor( conditionElement.attribute( "color" ) );

    QDomElement font = conditionElement.namedItem( "font" ).toElement();
    if ( !font.isNull() )
      newCondition.fontcond = new QFont( util_toFont( font ) );

    if ( conditionElement.hasAttribute( "style" ) )
    {
      newCondition.styleName = new QString( conditionElement.attribute( "style" ) );
      newCondition.style = manager->style( *newCondition.styleName );
      if ( !newCondition.style )
        ok = false;
    }

    if ( ok )
    {
      m_condList.append( newCondition );
    }
    else
    {
      kdDebug(36001) << "Error loading condition " << conditionElement.nodeName()<< endl;
    }
  }
}
