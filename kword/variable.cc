/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Variable						  */
/******************************************************************/

#include "variable.h"
#include "parag.h"
#include "defs.h"

#include <unistd.h>

/******************************************************************/
/* Class: KWVariablePgNumFormat					  */
/******************************************************************/

/*================================================================*/
void KWVariablePgNumFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariablePgNumFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_PGNUM )
    {
	warning( "Can't convert variable of type %d to a page num!!!", _var->getType() );
	return QString();
    }

    QString str;
    str.setNum( dynamic_cast<KWPgNumVariable*>( _var )->getPgNum() );
    str.prepend( pre );
    str.append( post );
    return QString( str );
}

/******************************************************************/
/* Class: KWVariableDateFormat					  */
/******************************************************************/

/*================================================================*/
void KWVariableDateFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableDateFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_DATE_FIX && _var->getType() != VT_DATE_VAR )
    {
	warning( "Can't convert variable of type %d to a date!!!", _var->getType() );
	return QString();
    }

    // for now...
    return dynamic_cast<KWDateVariable*>( _var )->getDate().toString();
}

/******************************************************************/
/* Class: KWVariableTimeFormat					  */
/******************************************************************/

/*================================================================*/
void KWVariableTimeFormat::setFormat( QString _format )
{
    KWVariableFormat::setFormat( _format );
}

/*================================================================*/
QString KWVariableTimeFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_TIME_FIX && _var->getType() != VT_TIME_VAR )
    {
	warning( "Can't convert variable of type %d to a time!!!", _var->getType() );
	return QString();
    }

    // for now...
    return dynamic_cast<KWTimeVariable*>( _var )->getTime().toString();
}

/******************************************************************/
/* Class: KWVariable						  */
/******************************************************************/

/*================================================================*/
QDomElement KWVariable::save( QDomDocument& doc )
{
    QDomElement var = doc.createElement( "VARIABLE" );
    var.setAttribute( "type", (int)getType() );

    QDomElement pos = doc.createElement( "POS" );
    var.appendChild( pos );
    pos.setAttribute( "frameSet", frameSetNum );
    pos.setAttribute( "frame", frameNum );
    pos.setAttribute( "pageNum", pageNum );

    return var;
}

/*================================================================*/
bool KWVariable::load( const QDomElement& element )
{
    frameSetNum = element.attribute( "frameSet" ).toInt();
    frameNum = element.attribute( "frame" ).toInt();
    pageNum = element.attribute( "page" ).toInt();
	    
    return TRUE;
}

/******************************************************************/
/* Class: KWPgNumVariable					  */
/******************************************************************/

/*================================================================*/
QDomElement KWPgNumVariable::save( QDomDocument& doc )
{
    QDomElement var = KWVariable::save( doc );

    QDomElement pgnum = doc.createElement( "PGNUM" );
    var.appendChild( pgnum );
    pgnum.setAttribute( "value", (int)pgNum );

    return var;
}

/*================================================================*/
bool KWPgNumVariable::load( const QDomElement& element )
{
    if ( !KWVariable::load( element ) )
	return FALSE;
    
    QDomElement e = element.namedItem( "PGNUM" ).toElement();
    if ( e.isNull() )
	return FALSE;
    pgNum = e.attribute( "value" ).toInt();
    
    return TRUE;
}

/******************************************************************/
/* Class: KWDateVariable					  */
/******************************************************************/

/*================================================================*/
KWDateVariable::KWDateVariable( KWordDocument *_doc, bool _fix, QDate _date )
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
QDomElement KWDateVariable::save( QDomDocument& doc )
{
    QDomElement var = KWVariable::save( doc );

    QDomElement dt = doc.createElement( "DATE" );
    var.appendChild( dt );
    dt.setAttribute( "year", date.year() );
    dt.setAttribute( "month", date.month() );
    dt.setAttribute( "day", date.day() );
    dt.setAttribute( "fix", fix );

    return var;
}

/*================================================================*/
bool KWDateVariable::load( const QDomElement& element )
{
    if ( !KWVariable::load( element ) )
	return FALSE;

    QDomElement e = element.namedItem( "DATE" ).toElement();
    fix = (bool)e.attribute( "fix" );
    
    int y = e.attribute( "year" ).toInt();
    int m = e.attribute( "month" ).toInt();
    int d = e.attribute( "day" ).toInt();
    date = QDate( y, m, d );
    
    return TRUE;
}

/******************************************************************/
/* Class: KWTimeVariable					  */
/******************************************************************/

/*================================================================*/
KWTimeVariable::KWTimeVariable( KWordDocument *_doc, bool _fix, QTime _time )
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
QDomElement KWTimeVariable::save( QDomDocument& doc )
{
    QDomElement var = KWVariable::save( doc );

    QDomElement tm = doc.createElement( "TIME" );
    var.appendChild( tm );
    tm.setAttribute( "hour", time.hour() );
    tm.setAttribute( "minute", time.minute() );
    tm.setAttribute( "second", time.second() );
    tm.setAttribute( "msecond", time.msec() );
    tm.setAttribute( "fix", fix );

    return var;
}

/*================================================================*/
bool KWTimeVariable::load( const QDomElement& element )
{
    if ( !KWVariable::load( element ) )
	return FALSE;

    QDomElement e = element.namedItem( "TIME" ).toElement();
    fix = (bool)e.attribute( "fix" );
    
    int h = e.attribute( "hour" ).toInt();
    int m = e.attribute( "minute" ).toInt();
    int s = e.attribute( "second" ).toInt();
    int ms = e.attribute( "second" ).toInt();
    time = QTime( h, m, s, ms );
    
    return TRUE;
}

