/* This file is part of the KDE project
   Copyright (C) 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   This is based off of the KOffice example found in the KOffice
   CVS.  Torben Weis <weis@kde.org> is the original author.

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

#include "kosoap_view.h"
#include "kosoap_factory.h"
#include "kosoap_part.h"

#include <qpainter.h>
#include <qiconset.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>

KOSoapView::KOSoapView( KOSoapPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KOSoapFactory::global() );
    setXMLFile( "kosoap.rc" );
    KStdAction::cut(this, SLOT( copy() ), actionCollection(), "copy" );
    KStdAction::redisplay( this, SLOT(refresh() ), actionCollection(), "reload");

    // Note: Prefer KStdAction::* to any custom action if possible.
    //m_cut = new KAction( i18n("&Cut"), "editcut", 0, this, SLOT( cut() ),
    //                   actionCollection(), "cut");
}

void KOSoapView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

void KOSoapView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KOSoapView::copy()
{
    kdDebug(31000) << "KOSoapView::cut(): CUT called" << endl;
}

void KOSoapView::refresh()
{
 kdDebug(31000) << "KOSoapView::refresh(): Refresh called" << endl;
}
#include "kosoap_view.moc"
