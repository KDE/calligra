/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright 2003 Nicolas GOUTTE <goutte@kde.org>

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

#include "config.h"
#include <koGlobal.h>
#include <kdebug.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kimageio.h>
#include <kiconloader.h>
#include <kstandarddirs.h>


KoGlobal* KoGlobal::s_global = 0L;
static KStaticDeleter<KoGlobal> sdg;

KoGlobal* KoGlobal::self()
{
    if ( !s_global )
        sdg.setObject( s_global, new KoGlobal );
    return s_global;
}

KoGlobal::KoGlobal()
    : m_pointSize( -1 ), m_kofficeConfig( 0L )
{
    // Install the libkoffice* translations
    KGlobal::locale()->insertCatalogue("koffice");

    KImageIO::registerFormats();

    // Tell KStandardDirs about the koffice prefix
    KGlobal::dirs()->addPrefix(PREFIX);

    // Tell the iconloader about share/apps/koffice/icons
    KGlobal::iconLoader()->addAppDir("koffice");

    // Another way to get the DPI of the display would be QPaintDeviceMetrics,
    // but we have no widget here (and moving this to KoView wouldn't allow
    // using this from the document easily).
#ifdef Q_WS_X11
    m_dpiX = QPaintDevice::x11AppDpiX();
    m_dpiY = QPaintDevice::x11AppDpiY();
#else
    m_dpiX = 75;
    m_dpiY = 75;
#endif
}

KoGlobal::~KoGlobal()
{
    delete m_kofficeConfig;
}

QFont KoGlobal::_defaultFont()
{
    QFont font = KGlobalSettings::generalFont();
    // we have to use QFontInfo, in case the font was specified with a pixel size
    if ( font.pointSize() == -1 )
    {
        // cache size into m_pointSize, since QFontInfo loads the font -> slow
        if ( m_pointSize == -1 )
            m_pointSize = QFontInfo(font).pointSize();
        Q_ASSERT( m_pointSize != -1 );
        font.setPointSize( m_pointSize );
    }
    //kdDebug()<<k_funcinfo<<"QFontInfo(font).pointSize() :"<<QFontInfo(font).pointSize()<<endl;
    //kdDebug()<<k_funcinfo<<"font.name() :"<<font.family ()<<endl;
    return font;
}

QStringList KoGlobal::_listTagOfLanguages()
{
    if ( m_languageTag.isEmpty() )
        createListOfLanguages();
    return m_languageTag;
}

QStringList KoGlobal::_listOfLanguages()
{
    if ( m_languageList.empty() )
        createListOfLanguages();
    return m_languageList;
}

void KoGlobal::createListOfLanguages()
{
    QStringList alllang = KGlobal::dirs()->findAllResources("locale",
                                                            QString::fromLatin1("*/entry.desktop"));
    QStringList langlist=alllang;
    for ( QStringList::ConstIterator it = langlist.begin();
          it != langlist.end(); ++it )
    {
        KSimpleConfig entry(*it);
        entry.setGroup("KCM Locale");
        QString name = entry.readEntry("Name",
                                       KGlobal::locale()->translate("without name"));

        QString tag = *it;
        int index = tag.findRev('/');
        tag = tag.left(index);
        index = tag.findRev('/');
        tag = tag.mid(index+1);
        m_languageList.append(name);
        m_languageTag.append(tag);
    }

}

QString KoGlobal::tagOfLanguage( const QString & _lang)
{
    // Should use iterator...
    int pos = self()->m_languageList.findIndex( _lang );
    if ( pos != -1)
    {
        return self()->m_languageTag[ pos ];
    }
    return QString::null;
}

int KoGlobal::languageIndexFromTag( const QString &_lang )
{
    return self()->m_languageTag.findIndex( _lang );
}

QString KoGlobal::languageFromTag( const QString &_lang )
{
    // should use iterator
    int pos = self()->m_languageTag.findIndex( _lang );
    if ( pos != -1)
        return self()->m_languageList[ pos ];
    else
        return QString::null;
}

KConfig* KoGlobal::_kofficeConfig()
{
    if ( !m_kofficeConfig ) {
        m_kofficeConfig = new KConfig( "kofficerc" );
    }
    return m_kofficeConfig;
}

void KoGlobal::setDPI( int x, int y )
{
    //kdDebug() << k_funcinfo << x << "," << y << endl;
    KoGlobal* s = self();
    s->m_dpiX = x;
    s->m_dpiY = y;
}
