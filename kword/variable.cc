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
#include "kwtextframeset.h"
#include "kwutils.h"
#include "serialletter.h"

#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/
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
QString KWVariableTimeFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_TIME_FIX && _var->getType() != VT_TIME_VAR ) {
        kdWarning() << "Can't convert variable of type " <<  _var->getType()  << " to a time!!!" << endl;
        return QString();
    }

    return KGlobal::locale()->formatTime( dynamic_cast<KWTimeVariable*>( _var )->getTime() );
}

/******************************************************************/
/* Class: KWVariableFieldFormat                                */
/******************************************************************/
QString KWVariableFieldFormat::convert( KWVariable *_var )
{
    KWFieldVariable * var = dynamic_cast<KWFieldVariable *>(_var);
    if ( var == 0L ) {
        kdWarning() << "Can't convert variable of type " << _var->getType() << " to a field!!!" << endl;
        return QString::null;
    }

    return var->value();
}

/******************************************************************/
/* Class: KWVariableCustomFormat                                  */
/******************************************************************/
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
QString KWVariableSerialLetterFormat::convert( KWVariable *_var )
{
    if ( _var->getType() != VT_SERIALLETTER ) {
        kdWarning() << "Can't convert variable of type " << _var->getType() << " to a page num!!!" << endl;
        return QString();
    }

    KWSerialLetterVariable * var = dynamic_cast<KWSerialLetterVariable*>( _var );
    QString value = var->getValue();
    if ( value == var->getName() )
        return "<" + value + ">";
    return value;
}


// ----------------------------------------------------------------------------------


/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/
KWVariable::KWVariable( KWTextFrameSet *fs, KWVariableFormat *_varFormat )
    : KWTextCustomItem( fs->textDocument() )
{
    varFormat = _varFormat;
    doc = fs->kWordDocument();
    doc->registerVariable( this );
}

KWVariable::~KWVariable()
{
    //kdDebug() << "KWVariable::~KWVariable " << this << endl;
    doc->unregisterVariable( this );
}

QTextFormat * KWVariable::format() const
{
    ASSERT( paragraph() );
    KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
    int index = parag->findCustomItem( this );
    //kdDebug() << "KWVariable::format index=" << index << " format=" << parag->at( index )->format() << endl;
    return parag->at( index )->format();
}

void KWVariable::resize()
{
    if ( m_deleted )
        return;
    QTextFormat fmt = *format(); // make an out-of-collection copy
    QTextString text;
    text.insert( 0, getText(), &fmt );
    width = 0;
    for ( uint i = 0 ; i < text.length() ; ++i )
        width += text.width( i );
    //kdDebug() << "KWVariable::resize width=" << width << endl;
}

void KWVariable::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg )
{
    QTextFormat * f = format();
    p->save();
    p->setPen( QPen( f->color() ) );
    p->setFont( f->font() );
    int bl, _y;
    KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
    int index = parag->findCustomItem( this );
    kdDebug() << "KWVariable::draw index=" << index << " x=" << x << " y=" << y << endl;
    parag->lineHeightOfChar( index, &bl, &_y );
    kdDebug() << "KWVariable::draw bl=" << bl << " _y=" << _y << endl;
    p->drawText( x, y /*+ _y*/ + bl, getText() );
    p->restore();
}

void KWVariable::save( QDomElement &formatElem )
{
    kdDebug() << "KWVariable::save" << endl;
    formatElem.setAttribute( "id", 4 ); // code for a variable
    QDomElement typeElem = formatElem.ownerDocument().createElement( "TYPE" );
    formatElem.appendChild( typeElem );
    typeElem.setAttribute( "type", static_cast<int>( getType() ) );
}

void KWVariable::load( QDomElement & )
{
}

//static
KWVariable * KWVariable::createVariable( int type, int subtype, KWTextFrameSet * textFrameSet )
{
    KWDocument * doc = textFrameSet->kWordDocument();
    KWVariableFormat * varFormat = doc->variableFormat( type );
    if ( !varFormat )
    {
        kdWarning() << "No variable format found for type " << (int)type << endl;
        return 0L;
    }
    KWVariable * var = 0L;
    switch ( type ) {
    case VT_DATE_FIX:
        var = new KWDateVariable( textFrameSet, TRUE, QDate::currentDate(), varFormat );
    break;
    case VT_DATE_VAR:
        var = new KWDateVariable( textFrameSet, FALSE, QDate::currentDate(), varFormat );
    break;
    case VT_TIME_FIX:
        var = new KWTimeVariable( textFrameSet, TRUE, QTime::currentTime(), varFormat );
    break;
    case VT_TIME_VAR:
        var = new KWTimeVariable( textFrameSet, FALSE, QTime::currentTime(), varFormat );
    break;
    case VT_PGNUM:
        var = new KWPgNumVariable( textFrameSet, varFormat );
    break;
    case VT_FIELD:
        var = new KWFieldVariable( textFrameSet, subtype, varFormat );
    break;
    case VT_CUSTOM:
       var = new KWCustomVariable( textFrameSet, QString::null, doc->variableFormat( type ) );
        break;
    case VT_SERIALLETTER:
        var = new KWSerialLetterVariable( textFrameSet, QString::null, doc->variableFormat( type ) );
        break;
    }
    return var;
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

void KWPgNumVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement pgNumElem = parentElem.ownerDocument().createElement( "PGNUM" );
    parentElem.appendChild( pgNumElem );
    pgNumElem.setAttribute( "value", pgNum );
}

void KWPgNumVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement pgNumElem = elem.namedItem( "PGNUM" ).toElement();
    if (!pgNumElem.isNull())
    {
        pgNum = pgNumElem.attribute("value").toInt();
    }
}

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

KWDateVariable::KWDateVariable( KWTextFrameSet *fs, bool _fix, QDate _date, KWVariableFormat *_varFormat )
    : KWVariable( fs, _varFormat ), date( _date ), fix( _fix )
{
    recalc();
}

void KWDateVariable::recalc()
{
    if ( !fix )
        date = QDate::currentDate();
}

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

void KWDateVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );

    QDomElement e = elem.namedItem( "DATE" ).toElement();
    if (!e.isNull())
    {
        int y = e.attribute("year").toInt();
        int m = e.attribute("month").toInt();
        int d = e.attribute("day").toInt();
        fix = static_cast<bool>( e.attribute("fix").toInt() );
        if ( fix )
            date.setYMD( y, m, d );
    }

    if ( !fix )
        date = QDate::currentDate();
}

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/

KWTimeVariable::KWTimeVariable( KWTextFrameSet *fs, bool _fix, QTime _time, KWVariableFormat *_varFormat )
    : KWVariable( fs, _varFormat ), time( _time ), fix( _fix )
{
    recalc();
}

void KWTimeVariable::recalc()
{
    if ( !fix )
        time = QTime::currentTime();
}

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

void KWTimeVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );

    QDomElement e = elem.namedItem( "TIME" ).toElement();
    if (!e.isNull())
    {
        int h = e.attribute("hour").toInt();
        int m = e.attribute("minute").toInt();
        int s = e.attribute("second").toInt();
        int ms = e.attribute("msecond").toInt();
        fix = static_cast<bool>( e.attribute("fix").toInt() );
        if ( fix )
            time.setHMS( h, m, s, ms );
    }

    if ( !fix )
        time = QTime::currentTime();
}

/******************************************************************/
/* Class: KWFieldVariable                                         */
/******************************************************************/
KWFieldVariable::KWFieldVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat )
    : KWVariable( fs, varFormat ), m_subtype( subtype )
{
    // m_subtype is VST_NONE when loading, it will be read in load()
    if ( m_subtype != VST_NONE )
        recalc();
}

void KWFieldVariable::save( QDomElement& parentElem )
{
    //kdDebug() << "KWFieldVariable::save" << endl;
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "FIELD" );
    parentElem.appendChild( elem );
    elem.setAttribute( "field", m_subtype );
    elem.setAttribute( "value", correctQString( m_value ) );
}

void KWFieldVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "FIELD" ).toElement();
    if (!e.isNull())
    {
        m_subtype = e.attribute( "field" ).toInt();
        m_value = e.attribute( "value" );
    } else
        kdWarning() << "FIELD element not found !" << endl;

    recalc();
}

void KWFieldVariable::recalc()
{
    switch( m_subtype ) {
        case VST_NONE:
            kdWarning() << "KWFieldVariable::recalc() called with m_subtype = VST_NONE !" << endl;
            break;
        case VST_FILENAME:
            m_value = doc->url().filename();
            break;
        case VST_AUTHORNAME:
        case VST_EMAIL:
        case VST_COMPANYNAME:
        {
            KConfig config( "kofficerc", true );
            if( config.hasGroup( "Author" ))
            {
                config.setGroup( "Author" );
                if ( m_subtype == VST_AUTHORNAME )
                    m_value = config.readEntry("full-name");
                else if ( m_subtype == VST_EMAIL )
                    m_value = config.readEntry("email");
                else if ( m_subtype == VST_COMPANYNAME )
                    m_value = config.readEntry("company");
            }
            else
            {
                KConfig config2( "emaildefaults", true );
                config2.setGroup( "Defaults" );
                QString group = config2.readEntry("Profile","Default");
                config2.setGroup(QString("PROFILE_%1").arg(group));
                if ( m_subtype == VST_AUTHORNAME )
                    m_value = config.readEntry("FullName");
                else if ( m_subtype == VST_EMAIL )
                    m_value = config.readEntry("EmailAddress");
            }
        }
    }

    if ( m_value.isEmpty() )
        m_value = i18n("<None>");
}


/******************************************************************/
/* Class: KWCustomVariable                                        */
/******************************************************************/
KWCustomVariable::KWCustomVariable( KWTextFrameSet *fs, const QString &name_, KWVariableFormat *_varFormat )
    : KWVariable( fs, _varFormat ), name( name_ )
{
    doc->unregisterVariable( this );
    doc->registerVariable( this );
    recalc();
}

void KWCustomVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "CUSTOM" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( name ) );
    elem.setAttribute( "value", correctQString( getValue() ) );
}

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

QString KWCustomVariable::getValue() const
{
    return doc->getVariableValue( name );
}

void KWCustomVariable::setValue( const QString &v )
{
    doc->setVariableValue( name, v );
}

/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/
KWSerialLetterVariable::KWSerialLetterVariable( KWTextFrameSet *fs, const QString &name_, KWVariableFormat *_varFormat )
    : KWVariable( fs, _varFormat ), name( name_ )
{
}

void KWSerialLetterVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "SERIALLETTER" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( name ) );
}

void KWSerialLetterVariable::load( QDomElement& elem )
{
    recalc();
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "SERIALLETTER" ).toElement();
    if (!e.isNull())
        name = e.attribute( "name" );
}

QString KWSerialLetterVariable::getValue() const
{
    return doc->getSerialLetterDataBase()->getValue( name );
}
