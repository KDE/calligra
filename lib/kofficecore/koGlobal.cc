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
#include <klocale.h>
#include <kprinter.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kimageio.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

struct PageFormatInfo
{
    KoFormat format;
    KPrinter::PageSize kprinter;
    const char* shortName; // Short name
    const char* descriptiveName; // Full name, which will be translated
    double width; // in mm
    double height; // in mm
};

// NOTES:
// - the width and height of non-ISO formats are rounded
// http://en.wikipedia.org/wiki/Paper_size can help
// - the comments "should be..." indicates the exact values if the inch sizes would be multiplied by 25.4 mm/inch

const PageFormatInfo pageFormatInfo[]=
{
    { PG_DIN_A3,        KPrinter::A3,           "A3",           I18N_NOOP("ISO A3"),       297.0,  420.0 },
    { PG_DIN_A4,        KPrinter::A4,           "A4",           I18N_NOOP("ISO A4"),       210.0,  297.0 },
    { PG_DIN_A5,        KPrinter::A5,           "A5",           I18N_NOOP("ISO A5"),       148.0,  210.0 },
    { PG_US_LETTER,     KPrinter::Letter,       "Letter",       I18N_NOOP("US Letter"),    215.9,  279.4 },
    { PG_US_LEGAL,      KPrinter::Legal,        "Legal",        I18N_NOOP("US Legal"),     215.9,  355.6 },
    { PG_SCREEN,        KPrinter::A4,           "Screen",       I18N_NOOP("Screen"), PG_A4_HEIGHT, PG_A4_WIDTH }, // Custom, so fall back to A4
    { PG_CUSTOM,        KPrinter::A4,           "Custom",       I18N_NOOP("Custom"), PG_A4_WIDTH, PG_A4_HEIGHT }, // Custom, so fall back to A4
    { PG_DIN_B5,        KPrinter::B5,           "B5",           I18N_NOOP("ISO B5"),       182.0,  257.0 },
    // Hmm, wikipedia says 184.15 * 266.7 for executive !
    { PG_US_EXECUTIVE,  KPrinter::Executive,    "Executive",    I18N_NOOP("US Executive"), 191.0,  254.0 }, // should be 190.5 mm x 254.0 mm
    { PG_DIN_A0,        KPrinter::A0,           "A0",           I18N_NOOP("ISO A0"),       841.0, 1189.0 },
    { PG_DIN_A1,        KPrinter::A1,           "A1",           I18N_NOOP("ISO A1"),       594.0,  841.0 },
    { PG_DIN_A2,        KPrinter::A2,           "A2",           I18N_NOOP("ISO A2"),       420.0,  594.0 },
    { PG_DIN_A6,        KPrinter::A6,           "A6",           I18N_NOOP("ISO A6"),       105.0,  148.0 },
    { PG_DIN_A7,        KPrinter::A7,           "A7",           I18N_NOOP("ISO A7"),        74.0,  105.0 },
    { PG_DIN_A8,        KPrinter::A8,           "A8",           I18N_NOOP("ISO A8"),        52.0,   74.0 },
    { PG_DIN_A9,        KPrinter::A9,           "A9",           I18N_NOOP("ISO A9"),        37.0,   52.0 },
    { PG_DIN_B0,        KPrinter::B0,           "B0",           I18N_NOOP("ISO B0"),      1030.0, 1456.0 },
    { PG_DIN_B1,        KPrinter::B1,           "B1",           I18N_NOOP("ISO B1"),       728.0, 1030.0 },
    { PG_DIN_B10,       KPrinter::B10,          "B10",          I18N_NOOP("ISO B10"),       32.0,   45.0 },
    { PG_DIN_B2,        KPrinter::B2,           "B2",           I18N_NOOP("ISO B2"),       515.0,  728.0 },
    { PG_DIN_B3,        KPrinter::B3,           "B3",           I18N_NOOP("ISO B3"),       364.0,  515.0 },
    { PG_DIN_B4,        KPrinter::B4,           "B4",           I18N_NOOP("ISO B4"),       257.0,  364.0 },
    { PG_DIN_B6,        KPrinter::B6,           "B6",           I18N_NOOP("ISO B6"),       128.0,  182.0 },
    { PG_ISO_C5,        KPrinter::C5E,          "C5",           I18N_NOOP("ISO C5"),       163.0,  229.0 }, // Some sources tells: 162 mm x 228 mm
    { PG_US_COMM10,     KPrinter::Comm10E,      "Comm10",       I18N_NOOP("US Common 10"), 105.0,  241.0 }, // should be 104.775 mm x 241.3 mm
    { PG_ISO_DL,        KPrinter::DLE,          "DL",           I18N_NOOP("ISO DL"),       110.0,  220.0 },
    { PG_US_FOLIO,      KPrinter::Folio,        "Folio",        I18N_NOOP("US Folio"),     210.0,  330.0 }, // should be 209.54 mm x 330.2 mm
    { PG_US_LEDGER,     KPrinter::Ledger,       "Ledger",       I18N_NOOP("US Ledger"),    432.0,  279.0 }, // should be 431.8 mm x 297.4 mm
    { PG_US_TABLOID,    KPrinter::Tabloid,      "Tabloid",      I18N_NOOP("US Tabloid"),   279.0,  432.0 }  // should be 297.4 mm x 431.8 mm
};

int KoPageFormat::printerPageSize( KoFormat format )
{
    if ( format == PG_SCREEN )
    {
            kdWarning() << "You use the page layout SCREEN. Printing in DIN A4 LANDSCAPE." << endl;
            return KPrinter::A4;
    }
    else if ( format == PG_CUSTOM )
    {
            kdWarning() << "The used page layout (CUSTOM) is not supported by KPrinter. Printing in A4." << endl;
            return KPrinter::A4;
    }
    else if ( format <= PG_LAST_FORMAT )
        return pageFormatInfo[ format ].kprinter;
    else
        return KPrinter::A4;
}

double KoPageFormat::width( KoFormat format, KoOrientation orientation )
{
    if ( orientation == PG_LANDSCAPE )
        return height( format, PG_PORTRAIT );
    if ( format <= PG_LAST_FORMAT )
        return pageFormatInfo[ format ].width;
    return PG_A4_WIDTH;   // should never happen
}

double KoPageFormat::height( KoFormat format, KoOrientation orientation )
{
    if ( orientation == PG_LANDSCAPE )
        return width( format, PG_PORTRAIT );
    if ( format <= PG_LAST_FORMAT )
        return pageFormatInfo[ format ].height;
    return PG_A4_HEIGHT;
}

KoFormat KoPageFormat::guessFormat( double width, double height )
{
    for ( int i = 0 ; i <= PG_LAST_FORMAT ; ++i )
    {
        // We have some tolerance. 1pt is a third of a mm, this is
        // barely noticeable for a page size.
        if ( i != PG_CUSTOM
             && kAbs( width - pageFormatInfo[i].width ) < 1.0
             && kAbs( height - pageFormatInfo[i].height ) < 1.0 )
            return static_cast<KoFormat>(i);
    }
    return PG_CUSTOM;
}

QString KoPageFormat::formatString( KoFormat format )
{
    if ( format <= PG_LAST_FORMAT )
        return QString::fromLatin1( pageFormatInfo[ format ].shortName );
    return QString::fromLatin1( "A4" );
}

KoFormat KoPageFormat::formatFromString( const QString & string )
{
    for ( int i = 0 ; i <= PG_LAST_FORMAT ; ++i )
    {
        if (string == QString::fromLatin1( pageFormatInfo[ i ].shortName ))
            return pageFormatInfo[ i ].format;
    }
    // We do not know the format name, so we have a custom format
    return PG_CUSTOM;
}

QString KoPageFormat::name( KoFormat format )
{
    if ( format <= PG_LAST_FORMAT )
        return i18n( pageFormatInfo[ format ].descriptiveName );
    return i18n( pageFormatInfo[ PG_DIN_A4 ].descriptiveName );
}

QStringList KoPageFormat::allFormats()
{
    QStringList lst;
    for ( int i = 0 ; i <= PG_LAST_FORMAT ; ++i )
    {
        lst << i18n( pageFormatInfo[ i ].descriptiveName );
    }
    return lst;
}

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
