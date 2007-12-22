/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "kounavail.h"

#include <qpainter.h>
#include <qicon.h>
#include <QPaintEvent>
#include <kaction.h>
#include <kcomponentdata.h>
#include <kstandardaction.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <qapplication.h>
#include <KoOdfReadStore.h>

KoUnavailPart::KoUnavailPart( QWidget *parentWidget, QObject* parent )
    : KoDocument( parentWidget, parent, false /*singleViewMode*/ )
{
    setReadWrite( false );
}

KoView* KoUnavailPart::createViewInstance( QWidget* parent)
{
    return new KoUnavailView( this, parent );
}

bool KoUnavailPart::loadOdf( KoOdfReadStore & odfStore )
{
    m_doc = KoXml::asQDomDocument( QDomDocument(), odfStore.contentDoc() );
    return true;
}

bool KoUnavailPart::loadXML( QIODevice *, const KoXmlDocument &doc )
{
    // Simply keep a copy of the whole document ;)
    m_doc = KoXml::asQDomDocument( QDomDocument(), doc );
    return true;
}

bool KoUnavailPart::saveFile()
{
    kDebug() <<"KoUnavailPart::saveFile m_url=" << url().prettyUrl();
    // This is called if the part points to an external file
    // In that case we have nothing to save, the file was unavailable !
    return true;
}

QDomDocument KoUnavailPart::saveXML()
{
    kDebug() <<"KoUnavailPart::saveXML";
    return m_doc;
}


bool KoUnavailPart::saveOdf( SavingContext & documentContext )
{
    Q_UNUSED( documentContext );
    // TODO
    return false;
}

void KoUnavailPart::setMimeType( const QByteArray& mime )
{
    kDebug() <<"KoUnavailPart::setMimeType" << mime;
    m_mimetype = mime;
}

void KoUnavailPart::paintContent( QPainter& painter, const QRect& rect)
{
    painter.save();
    painter.setPen( QApplication::palette().color( QPalette::Active, QColorGroup::Text ) );
    // Need to draw only the document rectangle described in the parameter rect.
    int left = rect.left() / 20;
    int right = rect.right() / 20 + 1;
    int top = rect.top() / 20;
    int bottom = rect.bottom() / 20 + 1;

    for( int x = left; x < right; ++x )
        painter.drawLine( x * 20, top * 20, x * 20, bottom * 20 );
    for( int y = left; y < right; ++y )
        painter.drawLine( left * 20, y * 20, right * 20, y * 20 );

    QFont defaultFont = KGlobalSettings::generalFont();
    defaultFont.setPointSize( 16 ); // ###
    painter.setFont( defaultFont );
    //painter.drawText( 20, 20, m_reason );
    painter.drawText( rect, Qt::AlignCenter | Qt::TextWordWrap, m_reason );
    painter.restore();
}

KoUnavailView::KoUnavailView( KoUnavailPart* part, QWidget* parent)
    : KoView( part, parent )
{
    setComponentData( KoUnavailFactory::global() );
    //setXMLFile( "kounavail.rc" );
}

void KoUnavailView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything( painter, ev->rect(), this );

    painter.end();
}

K_EXPORT_COMPONENT_FACTORY( libkounavailpart, KoUnavailFactory )

KComponentData* KoUnavailFactory::s_global = 0L;
KAboutData* KoUnavailFactory::s_aboutData = 0L;

KoUnavailFactory::KoUnavailFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KoUnavailFactory::~KoUnavailFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KoUnavailFactory::createPartObject( QWidget *parentWidget, QObject* parent, const char*, const QStringList & )
{
    return new KoUnavailPart( parentWidget, parent );
}

KAboutData* KoUnavailFactory::aboutData()
{
    if ( !s_aboutData )
    {
        static const char* description=I18N_NOOP("KoUnavail KOffice Program");
        static const char* version="0.1";
        s_aboutData=new KAboutData( "kounavail", 0, ki18n("KoUnavail"),
                                    version, ki18n(description), KAboutData::License_LGPL,
                                    ki18n("(c) 2001, David Faure"));
        s_aboutData->addAuthor(ki18n("David Faure"),KLocalizedString(), "faure@kde.org");
    }
    return s_aboutData;
}

const KComponentData &KoUnavailFactory::global()
{
    if ( !s_global )
    {
        s_global = new KComponentData( aboutData() );
        // Tell the iconloader about share/apps/koffice/icons
        //s_global->iconLoader()->addAppDir("koffice");
    }
    return *s_global;
}

#include "kounavail.moc"
