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
#include "koDocumentInfo.h"
#include "kwtextframeset.h"
#include "kwutils.h"
#include "serialletter.h"

#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>

QString KWVariableDateFormat::convert( const QDate & date )
{
    return KGlobal::locale()->formatDate( date );
}

QString KWVariableTimeFormat::convert( const QTime & time )
{
    return KGlobal::locale()->formatTime( time );
}

QString KWVariableStringFormat::convert( const QString & string )
{
    return string;
}

QString KWVariableNumberFormat::convert( int value /*double ? QVariant ?*/ )
{
    return QString::number( value );
}

/* for the prefix+suffix string format
    QString str;
    str.prepend( pre );
    str.append( post );
    return QString( str );
*/

/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/
KWVariable::KWVariable( KWTextFrameSet *fs, KWVariableFormat *varFormat )
    : KWTextCustomItem( fs->textDocument() )
{
    m_varFormat = varFormat;
    m_doc = fs->kWordDocument();
    m_doc->registerVariable( this );
}

KWVariable::~KWVariable()
{
    //kdDebug() << "KWVariable::~KWVariable " << this << endl;
    m_doc->unregisterVariable( this );
}

void KWVariable::resize()
{
    if ( m_deleted )
        return;
    QTextFormat *fmt = format();
    QString txt = text();
    width = 0;
    for ( int i = 0 ; i < (int)txt.length() ; ++i )
        width += fmt->width( txt, i );
    height = fmt->height();
    kdDebug() << "KWVariable::resize text=" << txt << " width=" << width << endl;
}

void KWVariable::draw( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected )
{
    QTextFormat * f = format();
    int bl, _y;
    KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
    //kdDebug() << "KWVariable::draw index=" << index() << " x=" << x << " y=" << y << endl;
    int h = parag->lineHeightOfChar( index(), &bl, &_y );

    p->save();
    p->setPen( QPen( f->color() ) );
    if ( selected )
    {
        p->setPen( QPen( cg.color( QColorGroup::HighlightedText ) ) );
        p->fillRect( x, y, width, h, cg.color( QColorGroup::Highlight ) );
    } else if ( parag->textDocument()->textFrameSet() &&
                parag->textDocument()->textFrameSet()->kWordDocument()->viewFormattingChars() && p->device()->devType() != QInternal::Printer )
    {
        p->setPen( QPen( cg.color( QColorGroup::Highlight ), 0, Qt::DotLine ) );
        p->drawRect( x, y, width, h );
    }
    p->setFont( f->font() );
    int offset=0;
    //code from qt3stuff
    if ( f->vAlign() == QTextFormat::AlignSuperScript )
    {
        QFont tmpFont( p->font() );
        tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
        p->setFont( tmpFont );
        offset=- ( h - p->fontMetrics().height() );
    }
    else if ( f->vAlign() == QTextFormat::AlignSubScript )
    {
        QFont tmpFont( p->font() );
        tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
        p->setFont( tmpFont );
    }

    //kdDebug() << "KWVariable::draw bl=" << bl << " _y=" << _y << endl;
    p->drawText( x, y /*+ _y*/ + bl+offset, text() );
    p->restore();
}

void KWVariable::save( QDomElement &formatElem )
{
    kdDebug() << "KWVariable::save" << endl;
    formatElem.setAttribute( "id", 4 ); // code for a variable
    QDomElement typeElem = formatElem.ownerDocument().createElement( "TYPE" );
    formatElem.appendChild( typeElem );
    typeElem.setAttribute( "type", static_cast<int>( type() ) );
}

void KWVariable::load( QDomElement & )
{
}

//static
KWVariable * KWVariable::createVariable( int type, int subtype, KWTextFrameSet * textFrameSet )
{
    KWDocument * doc = textFrameSet->kWordDocument();
    KWVariableFormat * varFormat = 0L;
    KWVariable * var = 0L;
    switch ( type ) {
        case VT_DATE:
            varFormat = doc->variableFormat( VF_DATE );
            var = new KWDateVariable( textFrameSet, subtype, varFormat );
            break;
        case VT_TIME:
            varFormat = doc->variableFormat( VF_TIME );
            var = new KWTimeVariable( textFrameSet, subtype, varFormat );
            break;
        case VT_PGNUM:
            varFormat = doc->variableFormat( VF_NUM );
            var = new KWPgNumVariable( textFrameSet, subtype, varFormat );
            break;
        case VT_FIELD:
            varFormat = doc->variableFormat( VF_STRING );
            var = new KWFieldVariable( textFrameSet, subtype, varFormat );
            break;
        case VT_CUSTOM:
            varFormat = doc->variableFormat( VF_STRING );
            var = new KWCustomVariable( textFrameSet, QString::null, varFormat );
            break;
        case VT_SERIALLETTER:
            varFormat = doc->variableFormat( VF_STRING );
            var = new KWSerialLetterVariable( textFrameSet, QString::null, varFormat );
            break;
    }
    return var;
}


/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/
KWDateVariable::KWDateVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *_varFormat )
    : KWVariable( fs, _varFormat ), m_subtype( subtype )
{
}

void KWDateVariable::recalc()
{
    if ( m_subtype == VST_DATE_CURRENT )
        m_date = QDate::currentDate();
    else
    {
        // Only if never set before (i.e. upon insertion)
        if ( m_date.isNull() )
            m_date = QDate::currentDate();
    }
    resize();
}

QString KWDateVariable::text()
{
    KWVariableDateFormat * format = dynamic_cast<KWVariableDateFormat *>( m_varFormat );
    ASSERT( format );
    if ( format )
        return format->convert( m_date );
    // make gcc happy
    return QString::null;
}

void KWDateVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );

    QDomElement elem = parentElem.ownerDocument().createElement( "DATE" );
    parentElem.appendChild( elem );
    elem.setAttribute( "year", m_date.year() );
    elem.setAttribute( "month", m_date.month() );
    elem.setAttribute( "day", m_date.day() );
    elem.setAttribute( "fix", m_subtype == VST_DATE_FIX ); // to be extended
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
        bool fix = e.attribute("fix").toInt() == 1;
        if ( fix )
            m_date.setYMD( y, m, d );
        m_subtype = fix ? VST_DATE_FIX : VST_DATE_CURRENT;
    }
}

QStringList KWDateVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Current date (fixed)" );
    lst << i18n( "Current date (variable)" );
    // TODO add date created, date printed, date last modified( BR #24242 )
    return lst;
}

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/
KWTimeVariable::KWTimeVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat )
    : KWVariable( fs, varFormat ), m_subtype( subtype )
{
}

void KWTimeVariable::recalc()
{
    if ( m_subtype == VST_TIME_CURRENT )
        m_time = QTime::currentTime();
    else
    {
        // Only if never set before (i.e. upon insertion)
        if ( m_time.isNull() )
            m_time = QTime::currentTime();
    }
    resize();
}

QString KWTimeVariable::text()
{
    KWVariableTimeFormat * format = dynamic_cast<KWVariableTimeFormat *>( m_varFormat );
    ASSERT( format );
    if ( format )
        return format->convert( m_time );
    // make gcc happy
    return QString::null;
}

void KWTimeVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );

    QDomElement elem = parentElem.ownerDocument().createElement( "TIME" );
    parentElem.appendChild( elem );
    elem.setAttribute( "hour", m_time.hour() );
    elem.setAttribute( "minute", m_time.minute() );
    elem.setAttribute( "second", m_time.second() );
    elem.setAttribute( "msecond", m_time.msec() );
    elem.setAttribute( "fix", m_subtype == VST_TIME_FIX );
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
        bool fix = static_cast<bool>( e.attribute("fix").toInt() );
        if ( fix )
            m_time.setHMS( h, m, s, ms );
        m_subtype = fix ? VST_TIME_FIX : VST_TIME_CURRENT;
    }
}

QStringList KWTimeVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Current time (fixed)" );
    lst << i18n( "Current time (variable)" );
    // TODO add time created, time printed, time last modified( BR #24242 )
    return lst;
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/
KWPgNumVariable::KWPgNumVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat )
        : KWVariable( fs, varFormat ), m_subtype( subtype ), m_pgNum( 0 )
{
}

void KWPgNumVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement pgNumElem = parentElem.ownerDocument().createElement( "PGNUM" );
    parentElem.appendChild( pgNumElem );
    pgNumElem.setAttribute( "subtype", m_subtype );
    pgNumElem.setAttribute( "value", m_pgNum );
}

void KWPgNumVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement pgNumElem = elem.namedItem( "PGNUM" ).toElement();
    if (!pgNumElem.isNull())
    {
        m_subtype = pgNumElem.attribute("subtype").toInt();
        m_pgNum = pgNumElem.attribute("value").toInt();
    }
}

void KWPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_CURRENT )
    {
#if 0 // Made obsolete by the (more dynamic) code in drawFrame.
        KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
        if ( !parag ) // too early
            return;
        KWTextFrameSet * fs = parag->textDocument()->textFrameSet();
        QPoint iPoint = parag->rect().topLeft(); // small bug if a paragraph is cut between two pages.
        QPoint cPoint;
        KWFrame * frame = fs->internalToNormal( iPoint, cPoint );
        if ( frame )
            m_pgNum = frame->pageNum() + 1;
#endif
    }
    else
        m_pgNum = m_doc->getPages();
    resize();
}

QString KWPgNumVariable::text()
{
    KWVariableNumberFormat * format = dynamic_cast<KWVariableNumberFormat *>( m_varFormat );
    ASSERT( format );
    if ( format )
        return format->convert( m_pgNum );
    // make gcc happy
    return QString::null;
}

QStringList KWPgNumVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "Page Number" );
    lst << i18n( "Number Of Pages" );
    return lst;
}

/******************************************************************/
/* Class: KWCustomVariable                                        */
/******************************************************************/
KWCustomVariable::KWCustomVariable( KWTextFrameSet *fs, const QString &name, KWVariableFormat *varFormat )
    : KWVariable( fs, varFormat ), m_name( name )
{
}

void KWCustomVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "CUSTOM" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( m_name ) );
    elem.setAttribute( "value", correctQString( value() ) );
}

void KWCustomVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "CUSTOM" ).toElement();
    if (!e.isNull())
    {
        m_name = e.attribute( "name" );
        setValue( e.attribute( "value" ) );
    }
}

QString KWCustomVariable::value() const
{
    return m_doc->getVariableValue( m_name );
}

void KWCustomVariable::setValue( const QString &v )
{
    m_doc->setVariableValue( m_name, v );
}

QStringList KWCustomVariable::actionTexts()
{
    return QStringList( i18n( "Custom..." ) );
}

void KWCustomVariable::recalc()
{
    resize();
}


/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/
KWSerialLetterVariable::KWSerialLetterVariable( KWTextFrameSet *fs, const QString &name, KWVariableFormat *varFormat )
    : KWVariable( fs, varFormat ), m_name( name )
{
}

void KWSerialLetterVariable::save( QDomElement& parentElem )
{
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "SERIALLETTER" );
    parentElem.appendChild( elem );
    elem.setAttribute( "name", correctQString( m_name ) );
}

void KWSerialLetterVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "SERIALLETTER" ).toElement();
    if (!e.isNull())
        m_name = e.attribute( "name" );
}

QString KWSerialLetterVariable::value() const
{
    return m_doc->getSerialLetterDataBase()->getValue( m_name );
}

QString KWSerialLetterVariable::text()
{
    // ## should use a format maybe
    QString v = value();
    if ( v == name() )
        return "<" + v + ">";
    return v;
}

QStringList KWSerialLetterVariable::actionTexts()
{
    return QStringList( i18n( "&Serial Letter..." ) );
}

/******************************************************************/
/* Class: KWFieldVariable                                         */
/******************************************************************/
KWFieldVariable::KWFieldVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat )
    : KWVariable( fs, varFormat ), m_subtype( subtype )
{
}

void KWFieldVariable::save( QDomElement& parentElem )
{
    //kdDebug() << "KWFieldVariable::save" << endl;
    KWVariable::save( parentElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "FIELD" );
    parentElem.appendChild( elem );
    elem.setAttribute( "subtype", m_subtype );
    elem.setAttribute( "value", correctQString( m_value ) );
}

void KWFieldVariable::load( QDomElement& elem )
{
    KWVariable::load( elem );
    QDomElement e = elem.namedItem( "FIELD" ).toElement();
    if (!e.isNull())
    {
        m_subtype = e.attribute( "subtype" ).toInt();
        if ( m_subtype == VST_NONE )
            kdWarning() << "Field subtype of -1 found in the file !" << endl;
        m_value = e.attribute( "value" );
    } else
        kdWarning() << "FIELD element not found !" << endl;
}

void KWFieldVariable::recalc()
{
    switch( m_subtype ) {
        case VST_NONE:
            kdWarning() << "KWFieldVariable::recalc() called with m_subtype = VST_NONE !" << endl;
            break;
        case VST_FILENAME:
            m_value = m_doc->url().filename();
            break;
        case VST_DIRECTORYNAME:
            m_value = m_doc->url().directory();
            break;
        case VST_AUTHORNAME:
        case VST_EMAIL:
        case VST_COMPANYNAME:
        {
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
            if ( !authorPage )
                kdWarning() << "Author information not found in documentInfo !" << endl;
            else
            {
                if ( m_subtype == VST_AUTHORNAME )
                    m_value = authorPage->fullName();
                else if ( m_subtype == VST_EMAIL )
                    m_value = authorPage->email();
                else if ( m_subtype == VST_COMPANYNAME )
                    m_value = authorPage->company();
            }
        }
        break;
        case VST_TITLE:
        case VST_ABSTRACT:
        {
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
            if ( !aboutPage )
                kdWarning() << "'About' page not found in documentInfo !" << endl;
            else
            {
                if ( m_subtype == VST_TITLE )
                    m_value = aboutPage->title();
                else
                    m_value = aboutPage->abstract();
            }
        }
        break;
    }

    if ( m_value.isEmpty() )
        m_value = i18n("<None>");
    resize();
}

QStringList KWFieldVariable::actionTexts()
{
    QStringList lst;
    lst << i18n( "File Name" );
    lst << i18n( "Directory Name" ); // is "Name" necessary ?
    lst << i18n( "Author Name" ); // is "Name" necessary ?
    lst << i18n( "Email" );
    lst << i18n( "Company Name" ); // is "Name" necessary ?
    lst << QString::null; //5
    lst << QString::null; //6
    lst << QString::null; //7
    lst << QString::null; //8
    lst << QString::null; //9
    lst << i18n( "Document Title" );
    lst << i18n( "Document Abstract" );
    return lst;
}
