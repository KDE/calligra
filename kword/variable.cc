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
// #### todo
// void KWVariable::load( string name, string tag, vector<KOMLAttrib>& lst )
// {
//     if ( name == "POS" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "frameSet" )
// 		frameSetNum = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "frame" )
// 		frameNum = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "pgNum" )
// 		pageNum = atoi( ( *it ).m_strValue.c_str() );
// 	}
//     }
// }

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
// #### todo
// void KWPgNumVariable::load( string name, string tag, vector<KOMLAttrib>& lst )
// {
//     KWVariable::load( name, tag, lst );

//     if ( name == "PGNUM" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "value" )
// 		pgNum = atoi( ( *it ).m_strValue.c_str() );
// 	}
//     }
// }

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
// #### todo
// void KWDateVariable::load( string name, string tag, vector<KOMLAttrib>& lst )
// {
//     KWVariable::load( name, tag, lst );

//     int y, m, d;

//     if ( name == "DATE" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "year" )
// 		y = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "month" )
// 		m = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "day" )
// 		d = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "fix" )
// 		fix = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
// 	}
//     }

//     if ( fix )
// 	date.setYMD( y, m, d );
//     else
// 	date = QDate::currentDate();
// }

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
// #### todo
// void KWTimeVariable::load( string name, string tag, vector<KOMLAttrib>& lst )
// {
//     KWVariable::load( name, tag, lst );

//     int h, m, s, ms;

//     if ( name == "TIME" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "hour" )
// 		h = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ((*it).m_strName == "minute")
// 		m = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "second" )
// 		s = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "msecond" )
// 		ms = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "fix" )
// 		fix = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
// 	}
//     }

//     if ( fix )
// 	time.setHMS( h, m, s, ms );
//     else
// 	time = QTime::currentTime();
// }


