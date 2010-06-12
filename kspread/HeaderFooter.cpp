/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "HeaderFooter.h"

#include "part/Doc.h" // FIXME detach from part
#include "Sheet.h"
#include "SheetPrint.h"

#include <KDebug>
#include <KLocale>
#include <KMessageBox>

#include <KoDocumentInfo.h>

#include <QDate>
#include <QPainter>
#include <QTime>

#include <pwd.h>
#include <unistd.h>


#define NO_MODIFICATION_POSSIBLE \
do { \
  KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) ); return; \
} while(0)


using namespace KSpread;

HeaderFooter::HeaderFooter(Sheet *sheet)
    : m_pSheet(sheet)
{
}

HeaderFooter::~HeaderFooter()
{
}

void HeaderFooter::replaceHeadFootLineMacro ( QString &_text, const QString &_search, const QString &_replace ) const
{
    if ( _search != _replace )
        _text.replace ( QString( '<' + _search + '>' ), '<' + _replace + '>' );
}

QString HeaderFooter::localizeHeadFootLine ( const QString &_text ) const
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, "page",   i18n("page") );
    replaceHeadFootLineMacro ( tmp, "pages",  i18n("pages") );
    replaceHeadFootLineMacro ( tmp, "file",   i18n("file") );
    replaceHeadFootLineMacro ( tmp, "name",   i18n("name") );
    replaceHeadFootLineMacro ( tmp, "time",   i18n("time") );
    replaceHeadFootLineMacro ( tmp, "date",   i18n("date") );
    replaceHeadFootLineMacro ( tmp, "author", i18n("author") );
    replaceHeadFootLineMacro ( tmp, "email",  i18n("email") );
    replaceHeadFootLineMacro ( tmp, "org",    i18n("org") );
    replaceHeadFootLineMacro ( tmp, "sheet",  i18n("sheet") );

    return tmp;
}

QString HeaderFooter::delocalizeHeadFootLine ( const QString &_text ) const
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, i18n("page"),   "page" );
    replaceHeadFootLineMacro ( tmp, i18n("pages"),  "pages" );
    replaceHeadFootLineMacro ( tmp, i18n("file"),   "file" );
    replaceHeadFootLineMacro ( tmp, i18n("name"),   "name" );
    replaceHeadFootLineMacro ( tmp, i18n("time"),   "time" );
    replaceHeadFootLineMacro ( tmp, i18n("date"),   "date" );
    replaceHeadFootLineMacro ( tmp, i18n("author"), "author" );
    replaceHeadFootLineMacro ( tmp, i18n("email"),  "email" );
    replaceHeadFootLineMacro ( tmp, i18n("org"),    "org" );
    replaceHeadFootLineMacro ( tmp, i18n("sheet"),  "sheet" );

    return tmp;
}

HeadFoot HeaderFooter::headFootLine() const
{
    HeadFoot hf;
    hf.headLeft  = m_headLeft;
    hf.headRight = m_headRight;
    hf.headMid   = m_headMid;
    hf.footLeft  = m_footLeft;
    hf.footRight = m_footRight;
    hf.footMid   = m_footMid;

    return hf;
}

void HeaderFooter::setHeadFootLine( const QString &_headl, const QString &_headm, const QString &_headr,
                                         const QString &_footl, const QString &_footm, const QString &_footr )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    m_headLeft  = _headl;
    m_headRight = _headr;
    m_headMid   = _headm;
    m_footLeft  = _footl;
    m_footRight = _footr;
    m_footMid   = _footm;
    if (m_pSheet->doc()) m_pSheet->doc()->setModified( true );
}

QString HeaderFooter::completeHeading( const QString &_data, int _page, const QString &_sheet ) const
{
    QString page( QString::number( _page) );
    QString pages(QString::number(m_pSheet->print()->pageCount()));

    QString pathFileName(m_pSheet->doc()->url().path());
    if ( pathFileName.isNull() )
        pathFileName="";

    QString fileName(m_pSheet->doc()->url().fileName());
    if( fileName.isNull())
        fileName="";

    QString t(QTime::currentTime().toString());
    QString d(QDate::currentDate().toString());
    QString ta;
    if ( !_sheet.isEmpty() )
        ta = _sheet;

    KoDocumentInfo* info = m_pSheet->doc()->documentInfo();
    QString full_name;
    QString email_addr;
    QString organization;
    QString tmp;
    if ( !info )
        kWarning() << "Author information not found in Document Info !";
    else
    {
        full_name = info->authorInfo( "creator" );
        email_addr = info->authorInfo( "email" );
        organization = info->authorInfo( "company" );
    }

    char hostname[80];
    struct passwd *p;

    p = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));

    if(full_name.isEmpty())
 	full_name=p->pw_gecos;

    if( email_addr.isEmpty())
	email_addr = QString("%1@%2").arg(p->pw_name).arg(hostname);

    tmp = _data;
    int pos = 0;
    while ( ( pos = tmp.indexOf( "<page>", pos ) ) != -1 )
        tmp.replace( pos, 6, page );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<pages>", pos ) ) != -1 )
        tmp.replace( pos, 7, pages );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<file>", pos ) ) != -1 )
        tmp.replace( pos, 6, pathFileName );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<name>", pos ) ) != -1 )
        tmp.replace( pos, 6, fileName );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<time>", pos ) ) != -1 )
        tmp.replace( pos, 6, t );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<date>", pos ) ) != -1 )
        tmp.replace( pos, 6, d );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<author>", pos ) ) != -1 )
        tmp.replace( pos, 8, full_name );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<email>", pos ) ) != -1 )
        tmp.replace( pos, 7, email_addr );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<org>", pos ) ) != -1 )
        tmp.replace( pos, 5, organization );
    pos = 0;
    while ( ( pos = tmp.indexOf( "<sheet>", pos ) ) != -1 )
        tmp.replace( pos, 7, ta );

    return tmp;
}

#include "HeaderFooter.moc"
