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
#include "parag.h"
#include "defs.h"
#include "kword_doc.h"
#include "kword_utils.h"
#include "serialletter.h"

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

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
KWVariable::KWVariable( KWordDocument *_doc )
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
void KWVariable::save( QTextStream&out )
{
    out << indent << "<TYPE type=\"" << static_cast<int>( getType() ) << "\"/>" << endl;
    out << indent << "<POS frameSet=\"" << frameSetNum << "\" frame=\"" << frameNum
        << "\" pageNum=\"" << pageNum << "\"/>" << endl;
}

/*================================================================*/
void KWVariable::load( KOMLParser& parser, QString name, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    if ( name == "POS" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "frameSet" )
                frameSetNum = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "frame" )
                frameNum = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "pgNum" )
                pageNum = ( *it ).m_strValue.toInt();
        }
    }
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

/*================================================================*/
void KWPgNumVariable::save( QTextStream&out )
{
    KWVariable::save( out );
    out << indent << "<PGNUM value=\"" << pgNum << "\"/>" << endl;
}

/*================================================================*/
void KWPgNumVariable::load( KOMLParser& parser, QString name, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    KWVariable::load( parser, name, tag, lst );

    if ( name == "PGNUM" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "value" )
                pgNum = ( *it ).m_strValue.toInt();
        }
    }
}

/******************************************************************/
/* Class: KWDateVariable                                          */
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
void KWDateVariable::save( QTextStream&out )
{
    KWVariable::save( out );
    out << indent << "<DATE year=\"" << date.year() << "\" month=\"" << date.month()
        << "\" day=\"" << date.day() << "\" fix=\"" << static_cast<int>( fix ) << "\"/>" << endl;
}

/*================================================================*/
void KWDateVariable::load( KOMLParser& parser, QString name, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    KWVariable::load( parser, name, tag, lst );

    int y, m, d;

    if ( name == "DATE" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "year" )
                y = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "month" )
                m = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "day" )
                d = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "fix" )
                fix = static_cast<bool>( ( *it ).m_strValue.toInt() );
        }
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
void KWTimeVariable::save( QTextStream&out )
{
    KWVariable::save( out );
    out << indent << "<TIME hour=\"" << time.hour() << "\" minute=\"" << time.minute()
        << "\" second=\"" << time.second() << "\" msecond=\"" << time.msec()
        << "\" fix=\"" << static_cast<int>( fix ) << "\"/>" << endl;
}

/*================================================================*/
void KWTimeVariable::load( KOMLParser& parser, QString name, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    KWVariable::load( parser, name, tag, lst );

    int h, m, s, ms;

    if ( name == "TIME" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for( ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "hour" )
                h = ( *it ).m_strValue.toInt();
            else if ((*it).m_strName == "minute")
                m = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "second" )
                s = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "msecond" )
                ms = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "fix" )
                fix = static_cast<bool>( ( *it ).m_strValue.toInt() );
        }
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
KWCustomVariable::KWCustomVariable( KWordDocument *_doc, const QString &name_ )
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
void KWCustomVariable::save( QTextStream&out )
{
    KWVariable::save( out );
    out << indent << "<CUSTOM name=\"" << correctQString( name ).latin1() << "\" value=\""
        << correctQString( getValue() ).latin1() << "\"/>" << endl;
}

/*================================================================*/
void KWCustomVariable::load( KOMLParser& parser, QString name_, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    doc->unregisterVariable( this );
    doc->registerVariable( this );
    recalc();
    KWVariable::load( parser, name_, tag, lst );
    if ( name_ == "CUSTOM" ) {
        parser.parseTag( tag, name_, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for(  ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "name" )
                name = ( *it ).m_strValue;
            else if ( (*it).m_strName == "value" )
                setValue( ( *it ).m_strValue );
        }
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
KWSerialLetterVariable::KWSerialLetterVariable( KWordDocument *_doc, const QString &name_ )
    : KWVariable( _doc ), name( name_ )
{
    recalc();
}

/*================================================================*/
void KWSerialLetterVariable::recalc()
{
}

/*================================================================*/
void KWSerialLetterVariable::save( QTextStream&out )
{
    KWVariable::save( out );
    out << indent << "<SERIALLETTER name=\"" << correctQString( name ).latin1()
        << "\"/>" << endl;
}

/*================================================================*/
void KWSerialLetterVariable::load( KOMLParser& parser, QString name_, const QString &tag, QValueList<KOMLAttrib>& lst )
{
    recalc();
    KWVariable::load( parser, name_, tag, lst );
    if ( name_ == "SERIALLETTER" ) {
        parser.parseTag( tag, name_, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for (  ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "name" )
                name = ( *it ).m_strValue;
        }
    }
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
