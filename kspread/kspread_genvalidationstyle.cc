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

#include "kspread_genvalidationstyle.h"
#include "kspread_cell.h"
#include <koxmlwriter.h>


KSpreadGenValidationStyles::KSpreadGenValidationStyles()
{

}

KSpreadGenValidationStyles::~KSpreadGenValidationStyles()
{

}

QString KSpreadGenValidationStyles::lookup( const KSpreadGenValidationStyle& style )
{
    StyleMap::iterator it = m_styles.find( style );
    if ( it == m_styles.end() ) {

        QString styleName( "val" );
        styleName = makeUniqueName( styleName );
        m_names.insert( styleName, true );
        it = m_styles.insert( style, styleName );
    }
    return it.data();
}

QString KSpreadGenValidationStyles::makeUniqueName( const QString& base ) const
{
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number( num++ );
    } while ( m_names.find( name ) != m_names.end() );
    return name;
}

void KSpreadGenValidationStyles::writeStyle( KoXmlWriter& writer )
{
    if ( m_styles.count()>0 )
    {
        writer.startElement( "table:content-validations" );
        StyleMap::Iterator it;
        for ( it = m_styles.begin(); it != m_styles.end(); ++it )
        {
            writer.startElement( "table:content-validation" );
            writer.addAttribute( "table:name", it.data() );
            writer.addAttribute( "table:allow-empty-cell", it.key().allowEmptyCell );
            writer.addAttribute( "table:condition", it.key().condition );

            writer.startElement( "table:help-message" );
            writer.addAttribute( "table:title", it.key().title );
            writer.addAttribute( "table:display", it.key().displayValidationInformation );

            QStringList text = QStringList::split( "\n", it.key().messageInfo );
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
            text = QStringList::split( "\n", it.key().message );
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

void KSpreadGenValidationStyle::initVal( KSpreadValidity *_val )
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
        case Warning:
            messageType = "warning";
            break;
        case Information:
            messageType = "information";
            break;
        case Stop:
            messageType = "stop";
            break;
        }

        titleInfo = _val->titleInfo;
        displayMessage = ( _val->displayMessage ? "true" : "false" );
        message = _val->message;
    }
}

QString KSpreadGenValidationStyle::createValidationCondition( KSpreadValidity* _val )
{
    QString result;
    switch( _val->m_allow )
    {
    case Allow_All:
        //nothing
        break;
    case Allow_Text:
        //doesn't exist into oo spec
        result = "cell-content-is-text()";
        break;
    case Allow_Time:
        result = createTimeValidationCondition( _val );
        break;
    case Allow_Date:
        result = createDateValidationCondition( _val );
        break;
    case Allow_Integer:
    case Allow_Number:
        result = createNumberValidationCondition( _val );
        break;
    case Allow_TextLength:
        result = createTextValidationCondition( _val );
         break;
    case Allow_List:
        result = createListValidationCondition( _val );
        break;
    }
    return result;
}

QString KSpreadGenValidationStyle::createListValidationCondition( KSpreadValidity* _val )
{
    QString result = "oooc:cell-content-is-in-list(";
    result = _val->listValidity.join( ";" );
    result +=")";
    return result;
}

QString KSpreadGenValidationStyle::createNumberValidationCondition( KSpreadValidity* _val )
{
    QString result;
    if ( _val->m_allow == Allow_Number )
        result = "oooc:cell-content-is-whole-number() and ";
    else if ( _val->m_allow == Allow_Integer )
        result = "oooc:cell-content-is-decimal-number() and ";
    switch( _val->m_cond )
    {
    case None:
        //nothing
        break;
    case Equal:
        result+="cell-content()";
        result+="=";
        result+=QString::number( _val->valMin );
        break;
    case Superior:
        result+="cell-content()";
        result+=">";
        result+=QString::number( _val->valMin );
        break;
    case Inferior:
        result+="cell-content()";
        result+="<";
        result+=QString::number( _val->valMin );
        break;
    case SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=QString::number( _val->valMin );
        break;
    case InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=QString::number( _val->valMin );
        break;
    case Different:
        result+="cell-content()";
        result+="!=";
        result+=QString::number( _val->valMin );
        break;
    case Between:
        result+="cell-content-is-between(";
        result+=QString::number( _val->valMin );
        result+=",";
        result+=QString::number( _val->valMax );
        result+=")";
        break;
    case DifferentTo:
        result+="cell-content-is-not-between(";
        result+=QString::number( _val->valMin );
        result+=",";
        result+=QString::number( _val->valMax );
        result+=")";
        break;
    }
    return result;
}


QString KSpreadGenValidationStyle::createTimeValidationCondition( KSpreadValidity* _val )
{
    QString result( "oooc:cell-content-is-time() and " );
    switch( _val->m_cond )
    {
    case None:
        //nothing
        break;
    case Equal:
        result+="cell-content()";
        result+="=";
        result+=_val->timeMin.toString( );
        break;
    case Superior:
        result+="cell-content()";
        result+=">";
        result+=_val->timeMin.toString( );
        break;
    case Inferior:
        result+="cell-content()";
        result+="<";
        result+=_val->timeMin.toString( );
        break;
    case SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=_val->timeMin.toString( );
        break;
    case InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=_val->timeMin.toString( );
        break;
    case Different:
        result+="cell-content()";
        result+="!=";
        result+=_val->timeMin.toString( );
        break;
    case Between:
        result+="cell-content-is-between(";
        result+=_val->timeMin.toString( );
        result+=",";
        result+=_val->timeMax.toString( );
        result+=")";
        break;
    case DifferentTo:
        result+="cell-content-is-not-between(";
        result+=_val->timeMin.toString( );
        result+=",";
        result+=_val->timeMax.toString( );
        result+=")";
        break;
    }
    return result;
}

QString KSpreadGenValidationStyle::createDateValidationCondition( KSpreadValidity* _val )
{
    QString result( "oooc:cell-content-is-date() and " );
    switch( _val->m_cond )
    {
    case None:
        //nothing
        break;
    case Equal:
        result+="cell-content()";
        result+="=";
        result+=_val->dateMin.toString();
        break;
    case Superior:
        result+="cell-content()";
        result+=">";
        result+=_val->dateMin.toString();
        break;
    case Inferior:
        result+="cell-content()";
        result+="<";
        result+=_val->dateMin.toString();
        break;
    case SuperiorEqual:
        result+="cell-content()";
        result+=">=";
        result+=_val->dateMin.toString();
        break;
    case InferiorEqual:
        result+="cell-content()";
        result+="<=";
        result+=_val->dateMin.toString();
        break;
    case Different:
        result+="cell-content()";
        result+="!=";
        result+=_val->dateMin.toString();
        break;
    case Between:
        result+="cell-content-is-between(";
        result+=_val->dateMin.toString();
        result+=",";
        result+=_val->dateMax.toString();
        result+=")";
        break;
    case DifferentTo:
        result+="cell-content-is-not-between(";
        result+=_val->dateMin.toString();
        result+=",";
        result+=_val->dateMax.toString();
        result+=")";
        break;
    }
    return result;
}

QString KSpreadGenValidationStyle::createTextValidationCondition( KSpreadValidity* _val )
{
    QString result;
    switch( _val->m_cond )
    {
    case None:
        //nothing
        break;
    case Equal:
        result+="oooc:cell-content-text-length()";
        result+="=";
        result+=QString::number( _val->valMin );
        break;
    case Superior:
        result+="oooc:cell-content-text-length()";
        result+=">";
        result+=QString::number( _val->valMin );
        break;
    case Inferior:
        result+="oooc:cell-content-text-length()";
        result+="<";
        result+=QString::number( _val->valMin );
        break;
    case SuperiorEqual:
        result+="oooc:cell-content-text-length()";
        result+=">=";
        result+=QString::number( _val->valMin );
        break;
    case InferiorEqual:
        result+="oooc:cell-content-text-length()";
        result+="<=";
        result+=QString::number( _val->valMin );
        break;
    case Different:
        result+="oooc:cell-content-text-length()";
        result+="!=";
        result+=QString::number( _val->valMin );
        break;
    case Between:
        result+="oooc:cell-content-text-length-is-between(";
        result+=QString::number( _val->valMin );
        result+=",";
        result+=QString::number( _val->valMax );
        result+=")";
        break;
    case DifferentTo:
        result+="oooc:cell-content-text-length-is-not-between(";
        result+=QString::number( _val->valMin );
        result+=",";
        result+=QString::number( _val->valMax );
        result+=")";
        break;
    }
    return result;
}
