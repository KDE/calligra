/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "view.h"
#include "factory.h"
#include "part.h"

#include <kprinter.h>
#include <kmessagebox.h>

#include "KoDocumentInfo.h"
#include <KoMainWindow.h>
#include <KoToolManager.h>
#include <KoToolBox.h>
#include <KoDocumentChild.h>

#include <QApplication>
#include <QDockWidget>
#include <QIcon>
#include <QLayout>
#include <QColor>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <qsize.h>
#include <QStackedWidget>
#include <QHeaderView>
#include <QRect>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QStyle>
#include <QVariant>

#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <k3command.h>
#include <ktoggleaction.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include <KoQueryTrader.h>

#include <assert.h>

namespace KPlatoWork
{

//-------------------------------
View::View( Part* part, QWidget* parent )
        : KoView( part, parent )
{
    //kDebug();

    setComponentData( Factory::global() );
    if ( !part->isReadWrite() )
        setXMLFile( "kplatowork_readonly.rc" );
    else
        setXMLFile( "kplatowork.rc" );

    QTableView *v = new QTableView( this );
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin(0);
    layout->addWidget( v );

    QStandardItemModel *m = new QStandardItemModel( this );
    QStringList lab; lab << "C1";
    m->setHorizontalHeaderLabels( lab );
    m->appendRow( new QStandardItem( "Test" ) );
    v->setModel( m );
    
    //kDebug()<<" end";
}

View::~View()
{
}

Part *View::part() const
{
    return static_cast<Part*>( koDocument() );
}

void View::slotPopupMenu( const QString& menuname, const QPoint & pos )
{
    QMenu * menu = this->popupMenu( menuname );
    if ( menu ) {
        //kDebug()<<menu<<":"<<menu->actions().count();
        menu->exec( pos );
    }
}


}  //KPlatoWP namespace

#include "view.moc"
