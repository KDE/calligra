/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "kptview.h"
#include "kptfactory.h"
#include "kptpart.h"

#include <qpainter.h>
#include <qiconset.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>

KPTView::KPTView( KPTPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KPTFactory::global() );
    setXMLFile( "kplato.rc" );
    KStdAction::cut(this, SLOT( cut() ), actionCollection(), "cut" );
    // Note: Prefer KStdAction::* to any custom action if possible.
    //m_cut = new KAction( i18n("&Cut"), "editcut", 0, this, SLOT( cut() ),
    //                   actionCollection(), "cut");

    QBoxLayout *l = new QHBoxLayout(this);
    l->setAutoAdd(true);

    listview = new KListView(this);
    // listview->setRootIsDecorated(true);
    listview->addColumn("Project");
    KListViewItem *i = new KListViewItem(listview, "item 1");
    new KListViewItem(i, "item 2");
}

// void KPTView::paintEvent( QPaintEvent* ev )
// {
//     QPainter painter;
//     painter.begin( this );

//     // ### TODO: Scaling

//     // Let the document do the drawing
//     koDocument()->paintEverything( painter, ev->rect(), FALSE, this );

//     painter.end();
// }

void KPTView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KPTView::cut()
{
    kdDebug(31000) << "KPTView::cut(): CUT called" << endl;
}

#include "kptview.moc"
