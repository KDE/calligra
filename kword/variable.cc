/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "variable.h"
#include "defs.h"
#include "kwdoc.h"
#include "kwutils.h"
#include "serialletter.h"

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/

/*================================================================*/
void KWVariablePgNumFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariablePgNumFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_PGNUM ) {
        kdWarning() << "Can't convert variable of type " <<  _var->getType()  << " to a page num!!!" << endl;
        return QString();
    }

    QString str;
    str.setNum( dynamic_cast<KWPgNumVariable*>( _var )->getPgNum() );
    str.prepend( pre );
    str.append( post );
    return QString( str );
}

/******************************************************************/
/* Class: KWVariableDateFormat                                    */
/******************************************************************/

/*================================================================*/
void KWVariableDateFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableDateFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_DATE_FIX && _var->getType() != VT_DATE_VAR ) {
        kdWarning() << "Can't convert variable of type " <<  _var->getType()  << " to a date!!!" << endl;
        return QString();
    }

    return KGlobal::locale()->formatDate( dynamic_cast<KWDateVariable*>( _var )->getDate() );
}

/******************************************************************/
/* Class: KWVariableTimeFormat                                    */
/******************************************************************/

/*================================================================*/
void KWVariableTimeFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableTimeFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_TIME_FIX && _var->getType() != VT_TIME_VAR ) {
        kdWarning() << "Can't convert variable of type " <<  _var->getType()  << " to a time!!!" << endl;
        return QString();
    }

    return KGlobal::locale()->formatTime( dynamic_cast<KWTimeVariable*>( _var )->getTime() );
}

/******************************************************************/
/* Class: KWVariableCustomFormat                                  */
/******************************************************************/

/*================================================================*/
void KWVariableCustomFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableCustomFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_CUSTOM ) {
        kdWarning() << "Can't convert variable of type " << _var->getType() << " to a page num!!!" << endl;
        return QString();
    }

    return dynamic_cast<KWCustomVariable*>( _var )->getValue();
}

/******************************************************************/
/* Class: KWVariableSerialLetterFormat                            */
/******************************************************************/

/*================================================================*/
void KWVariableSerialLetterFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableSerialLetterFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_SERIALLETTER ) {
        kdWarning() << "Can't convert variable of type " << _var->getType() << " to a page num!!!" << endl;
        return QString();
    }

    if ( dynamic_cast<KWSerialLetterVariable*>( _var )->getValue() ==
         dynamic_cast<KWSerialLetterVariable*>( _var )->getName() )
        return "<" + dynamic_cast<KWSerialLetterVariable*>( _var )->getValue() + ">";
    return dynamic_cast<KWSerialLetterVariable*>( _var )->getValue();
}


// ----------------------------------------------------------------------------------


/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/

/*================================================================*/
KWVariable::KWVariable( KWDocument *_doc )
    : text()
{
    varFormat = 0L;
    doc = _doc;
    doc->registerVariable( this );
}

/*================================================================*/
KWVariable::~KWVariable()
{
    doc->unregisterVariable( this );
}

/*================================================================*/
void KWVariable::save( QDomElement &parentElem )
{
    QDomElement typeElem = parentElem.ownerDocument().createElement( "TYPE" );
    parentElem.appendChild( typeElem );
    typeElem.setAttribute( "type", static_cast<int>( getType() ) );
    QDomElement posElem = parentElem.ownerDocument().createElement( "POS" );
    parentElem.appendChild( typeElem );
    typeElem.setAttribute( "frameSet", frameSetNum );
    typeElem.setAttribute( "frame", frameNum );
    typeElem.setAttribute( "pageNum", pageNum );
}

/*================================================================*/
void KWVariable::load( QDomElement & elem )
{
    QDomElement posElem = elem.namedItem("POS").toElement();
    if (!posElem.isNull())
    {
        frameSetNum = posElem.attribute( "frameSet" ).toInt();
        frameNum = posElem.attribute( "frame" ).toInt();
        pageNum = posElem.attribute( "pgNum" ).toInt();
    }
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

/*================================================================*/
void KWPgNumVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement pgNumElem = parentElem.ownerDocument().createElement( "PGNUM" );
    parentElem.appendChild( pgNumElem );
    pgNumElem.setAttribute( "value", pgNum );
}

/*================================================================*/
void KWPgNumVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement pgNumElem = elem.namedItem( "PGNUM" );
    if (!pgNumElem.isNull())
    {
        pgNum = pgNumElem.attribute("value").toInt();
    }
}

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

/*================================================================*/
KWDateVariable::KWDateVariable( KWDocument *_doc, bool _fix, QDate _date )
    : KWVariable( _doc ), fix( _fix )
{
    if ( !fix )
        date = QDate::currentDate();
    else
        date = _date;

    recalc();
}

/*================================================================*/
void KWDateVariable::recalc()
{
}

/*================================================================*/
void KWDateVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    
    QDomElement elem = parentElem.ownerDocument().createElement( "DATE" );
    parentElem.appendChild( elem );
    elem.setAttribute( "year", date.year() );
    elem.setAttribute( "month", date.month() );
    elem.setAttribute( "day", date.day() );
    elem.setAttribute( "fix", static_cast<int>( fix ) );
}

/*================================================================*/
void KWDateVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );

    int y, m, d;

    QDomElement e = elem.namedItem( "DATE" );
    if (!e.isNull())
    {
        y = e.attribute("year").toInt();
        m = e.attribute("month").toInt();
        d = e.attribute("day").toInt();
        fix = static_cast<bool>( e.attribute("fix").toInt() );
    }

    if ( fix )
        date.setYMD( y, m, d );
    else
        date = QDate::currentDate();
}

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/

/*================================================================*/
KWTimeVariable::KWTimeVariable( KWDocument *_doc, bool _fix, QTime _time )
    : KWVariable( _doc ), fix( _fix )
{
    if ( !fix )
        time = QTime::currentTime();
    else
        time = _time;

    recalc();
}

/*================================================================*/
void KWTimeVariable::recalc()
{
}

/*================================================================*/
void KWTimeVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    
    QDomElement elem = parentElem.ownerDocument().createElement( "TIME" );
    parentElem.appendChild( elem );
    elem.setAttribute( "hour", time.hour() );
    elem.setAttribute( "minute", time.minute() );
    elem.setAttribute( "second", time.second() );
    elem.setAttribute( "msecond", time.msec() );
    elem.setAttribute( "fix", static_cast<int>( fix ) );
}

/*================================================================*/
void KWTimeVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );

    int h, m, s, ms;
    QDomElement e = elem.namedItem( "TIME" );
    if (!e.isNull())
    {
        h = e.attribute("hour").toInt();
        m = e.attribute("minute").toInt();
        s = e.attribute("second").toInt();
        ms = e.attribute("msecond").toInt();
        fix = static_cast<bool>( e.attribute("fix").toInt() );
    }

    if ( fix )
        time.setHMS( h, m, s, ms );
    else
        time = QTime::currentTime();
}

/******************************************************************/
/* Class: KWCustomVariable                                        */
/******************************************************************/

/*================================================================*/
KWCustomVariable::KWCustomVariable( KWDocument *_doc, const QString &name_ )
    : KWVariable( _doc ), name( name_ )
{
    doc->unregisterVariable( this );
    doc->registerVariable( this );
    recalc();
}

/*================================================================*/
void KWCustomVariable::recalc()
{
}

/*================================================================*/
void KWCustomVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "CUSTOM" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( name ) );
    elem.setAttribute( "value", correctQString( getValue() ) );
}

/*================================================================*/
void KWCustomVariable::load( QDomElement& elem )
{
    doc->unregisterVariable( this );
    doc->registerVariable( this );
    recalc();
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "CUSTOM" ).toElement();
    if (!e.isNull())
    {
        name = e.attribute( "name" );
        setValue( e.attribute( "value" ) );
    }
}

/*================================================================*/
QString KWCustomVariable::getName() const
{
    return name;
}

/*================================================================*/
QString KWCustomVariable::getValue() const
{
    return doc->getVariableValue( name );
}

/*================================================================*/
void KWCustomVariable::setValue( const QString &v )
{
    doc->setVariableValue( name, v );
}

/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/

/*================================================================*/
KWSerialLetterVariable::KWSerialLetterVariable( KWDocument *_doc, const QString &name_ )
    : KWVariable( _doc ), name( name_ )
{
    recalc();
}

/*================================================================*/
void KWSerialLetterVariable::recalc()
{
}

/*================================================================*/
void KWSerialLetterVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "SERIALLETTER" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( name ) );
}

/*================================================================*/
void KWSerialLetterVariable::load( QDomElement& elem )
{
    recalc();
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "SERIALLETTER" ).toElement();
    if (!e.isNull())
        name = e.attribute( "name" );
}

/*================================================================*/
QString KWSerialLetterVariable::getName() const
{
    return name;
}

/*================================================================*/
QString KWSerialLetterVariable::getValue() const
{
    return doc->getSerialLetterDataBase()->getValue( name );
}
