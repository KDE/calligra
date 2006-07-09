/* This file is part of the KDE project

   Copyright 2004 Laurent Montel <montel@kde.org>

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

#include "Genvalidationstyle.h"
#include "Cell.h"
#include <KoXmlWriter.h>

using namespace KSpread;

GenValidationStyles::GenValidationStyles()
{

}

GenValidationStyles::~GenValidationStyles()
{

}

QString GenValidationStyles::lookup( const GenValidationStyle& style )
{
    StyleMap::iterator it = m_styles.find( style );
    if ( it == m_styles.end() ) {

        QString styleName( "val" );
        styleName = makeUniqueName( styleName );
        m_names.insert( styleName, true );
        it = m_styles.insert( style, styleName );
    }
    return it.value();
}

QString GenValidationStyles::makeUniqueName( const QString& base ) const
{
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number( num++ );
    } while ( m_names.find( name ) != m_names.end() );
    return name;
}

void GenValidationStyles::writeStyle( KoXmlWriter& writer )
{
    if ( m_styles.count()>0 )
    {
        writer.startElement( "table:content-validations" );
        StyleMap::Iterator it;
        for ( it = m_styles.begin(); it != m_styles.end(); ++it )
        {
            writer.startElement( "table:content-validation" );
            writer.addAttribute( "table:name", it.value() );
            writer.addAttribute( "table:allow-empty-cell", it.key().allowEmptyCell );
            writer.addAttribute( "table:condition", it.key().condition );

            writer.startElement( "table:help-message" );
            writer.addAttribute( "table:title", it.key().title );
            writer.addAttribute( "table:display", it.key().displayValidationInformation );

            QStringList text = it.key().messageInfo.split( "\n", QString::SkipEmptyParts );
            for ( QStringList::Iterator it2 = text.begin(); it2 != text.end(); ++it2 ) {
                writer.startElement( "text:p" );
                writer.addTextNode( *it2 );
                writer.endElement();
            }
            writer.endElement();

            writer.startElement( "table:error-message" );
            writer.addAttribute( "table:message-type", it.key().messageType );

            writer.addAttribute("table:title", it.key().titleInfo);
            writer.addAttribute("table:display", it.key().displayMessage);
            text = it.key().message.split( "\n", QString::SkipEmptyParts );
            for ( QStringList::Iterator it3 = text.begin(); it3 != text.end(); ++it3 ) {
                writer.startElement( "text:p" );
                writer.addTextNode( *it3 );
                writer.endElement();
            }
            writer.endElement();

            writer.endElement();
        }
        writer.endElement();//close sheet:content-validation
    }
}

void GenValidationStyle::initVal( Validity *_val )
{
    if ( _val )
    {
        allowEmptyCell = ( _val->allowEmptyCell ? "true" : "false" );
        condition = createValidationCondition( _val );
        title = _val->title;
        displayValidationInformation = ( _val->displayValidationInformation ? "true" : "false" );
        messageInfo = _val->messageInfo;

        switch( _val->m_action )
        {
          case Action::Warning:
            messageType = "warning";
            break;
          case Action::Information:
            messageType = "information";
            break;
          case Action::Stop:
            messageType = "stop";
            break;
        }

        titleInfo = _val->titleInfo;
        displayMessage = ( _val->displayMessage ? "true" : "false" );
        message = _val->message;
    }
}

QString GenValidationStyle::createValidationCondition( Validity* _val )
{
    QString result;
    switch( _val->m_restriction )
    {
    case Restriction::None:
        //nothing
        break;
    case Restriction::Text:
        //doesn't exist into oo spec
        result = "cell-content-is-text()";
        break;
    case Restriction::Time:
        result = createTimeValidationCondition( _val );
        break;
    case Restriction::Date:
        result = createDateValidationCondition( _val );
        break;
    case Restriction::Integer:
    case Restriction::Number:
        result = createNumberValidationCondition( _val );
        break;
    case Restriction::TextLength:
        result = createTextValidationCondition( _val );
         break;
    case Restriction::List:
        result = createListValidationCondition( _val );
        break;
    }
    return result;
}

QString GenValidationStyle::createListValidationCondition( Validity* _val )
{
    QString result = "oooc:cell-content-is-in-list(";
    result = _val->listValidity.join( ";" );
    result +=')';
    return result;
}

QString GenValidationStyle::createNumberValidationCondition( Validity* _val )
{
    QString result;
    if ( _val->m_restriction == Restriction::Number )
        result = "oooc:cell-content-is-whole-number() and ";
    else if ( _val->m_restriction == Restriction::Integer )
        result = "oooc:cell-content-is-decimal-number() and ";
    switch( _val->m_cond )
    {
      case Conditional::None:
        //nothing
        break;
      case Conditional::Equal:
        result+="cell-content()";
        result+='=';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Superior:
        result+="cell-content()";
        result+='>';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Inferior:
        result+="cell-content()";
        result+='<';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Different:
        result+="cell-content()";
        result+="!=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Between:
        result+="cell-content-is-between(";
        result+=QString::number( _val->valMin );
        result+=',';
        result+=QString::number( _val->valMax );
        result+=')';
        break;
      case Conditional::DifferentTo:
        result+="cell-content-is-not-between(";
        result+=QString::number( _val->valMin );
        result+=',';
        result+=QString::number( _val->valMax );
        result+=')';
        break;
    }
    return result;
}


QString GenValidationStyle::createTimeValidationCondition( Validity* _val )
{
    QString result( "oooc:cell-content-is-time() and " );
    switch( _val->m_cond )
    {
      case Conditional::None:
        //nothing
        break;
      case Conditional::Equal:
        result+="cell-content()";
        result+='=';
        result+=_val->timeMin.toString( );
        break;
      case Conditional::Superior:
        result+="cell-content()";
        result+='>';
        result+=_val->timeMin.toString( );
        break;
      case Conditional::Inferior:
        result+="cell-content()";
        result+='<';
        result+=_val->timeMin.toString( );
        break;
      case Conditional::SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=_val->timeMin.toString( );
        break;
      case Conditional::InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=_val->timeMin.toString( );
        break;
      case Conditional::Different:
        result+="cell-content()";
        result+="!=";
        result+=_val->timeMin.toString( );
        break;
      case Conditional::Between:
        result+="cell-content-is-between(";
        result+=_val->timeMin.toString( );
        result+=',';
        result+=_val->timeMax.toString( );
        result+=')';
        break;
      case Conditional::DifferentTo:
        result+="cell-content-is-not-between(";
        result+=_val->timeMin.toString( );
        result+=',';
        result+=_val->timeMax.toString( );
        result+=')';
        break;
    }
    return result;
}

QString GenValidationStyle::createDateValidationCondition( Validity* _val )
{
    QString result( "oooc:cell-content-is-date() and " );
    switch( _val->m_cond )
    {
      case Conditional::None:
        //nothing
        break;
      case Conditional::Equal:
        result+="cell-content()";
        result+='=';
        result+=_val->dateMin.toString();
        break;
      case Conditional::Superior:
        result+="cell-content()";
        result+='>';
        result+=_val->dateMin.toString();
        break;
      case Conditional::Inferior:
        result+="cell-content()";
        result+='<';
        result+=_val->dateMin.toString();
        break;
      case Conditional::SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=_val->dateMin.toString();
        break;
      case Conditional::InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=_val->dateMin.toString();
        break;
      case Conditional::Different:
        result+="cell-content()";
        result+="!=";
        result+=_val->dateMin.toString();
        break;
      case Conditional::Between:
        result+="cell-content-is-between(";
        result+=_val->dateMin.toString();
        result+=',';
        result+=_val->dateMax.toString();
        result+=')';
        break;
      case Conditional::DifferentTo:
        result+="cell-content-is-not-between(";
        result+=_val->dateMin.toString();
        result+=',';
        result+=_val->dateMax.toString();
        result+=')';
        break;
    }
    return result;
}

QString GenValidationStyle::createTextValidationCondition( Validity* _val )
{
    QString result;
    switch( _val->m_cond )
    {
      case Conditional::None:
        //nothing
        break;
      case Conditional::Equal:
        result+="oooc:cell-content-text-length()";
        result+='=';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Superior:
        result+="oooc:cell-content-text-length()";
        result+='>';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Inferior:
        result+="oooc:cell-content-text-length()";
        result+='<';
        result+=QString::number( _val->valMin );
        break;
      case Conditional::SuperiorEqual:
        result+="oooc:cell-content-text-length()";
        result+=">=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::InferiorEqual:
        result+="oooc:cell-content-text-length()";
        result+="<=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Different:
        result+="oooc:cell-content-text-length()";
        result+="!=";
        result+=QString::number( _val->valMin );
        break;
      case Conditional::Between:
        result+="oooc:cell-content-text-length-is-between(";
        result+=QString::number( _val->valMin );
        result+=',';
        result+=QString::number( _val->valMax );
        result+=')';
        break;
      case Conditional::DifferentTo:
        result+="oooc:cell-content-text-length-is-not-between(";
        result+=QString::number( _val->valMin );
        result+=',';
        result+=QString::number( _val->valMax );
        result+=')';
        break;
    }
    return result;
}
