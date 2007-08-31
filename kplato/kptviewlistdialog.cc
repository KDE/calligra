/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptviewlistdialog.h"
#include "kptview.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

// little helper stolen from kmail/kword
static inline QPixmap loadIcon( const char * name ) {
  return KIconLoader::global()->loadIcon( QString::fromLatin1(name), K3Icon::NoGroup, K3Icon::SizeMedium );
}


ViewListDialog::ViewListDialog( View *view, ViewListWidget &viewlist, QWidget *parent )
    : KDialog(parent)
{
    setCaption( i18n("Add View") );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setDefaultButton( Ok );

    m_panel = new AddViewPanel( view, viewlist, this );

    setMainWidget( m_panel );
    
    enableButtonOk(false);

    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
    connect( m_panel, SIGNAL( enableButtonOk( bool ) ), SLOT( enableButtonOk( bool ) ) );
}


void ViewListDialog::slotOk() {
    if ( m_panel->ok() ) {
        accept();
    }
}

//------------------------
AddViewPanel::AddViewPanel( View *view, ViewListWidget &viewlist, QWidget *parent )
    : QWidget( parent ),
      m_view( view ),
      m_viewlist( viewlist )
{
    widget.setupUi( this );
    
    // NOTE: must match switch in ok()
    QStringList lst;
    lst << i18n( "Resource editor" )
            << i18n( "Task editor" )
            << i18n( "Task status" );
    widget.viewtype->addItems( lst );
    
    foreach ( ViewListItem *item, m_viewlist.categories() ) {
        m_categories.insert( item->text( 0 ), item->tag() );
    }
    widget.category->addItems( m_categories.keys() );
    
    connect( widget.viewname, SIGNAL( textChanged( const QString& ) ), SLOT( changed() ) );
    connect( widget.tooltip, SIGNAL( textChanged( const QString& ) ), SLOT( changed() ) );
}

bool AddViewPanel::ok()
{
    QString n = widget.category->currentText();
    QString c = m_categories.value( n );
    if ( c.isEmpty() ) {
        c = n;
    }
    ViewListItem *cat = m_viewlist.addCategory( c, n );
    if ( cat == 0 ) {
        return false;
    }
    switch ( widget.viewtype->currentIndex() ) {
        case 0: // Resource editor
            m_view->createResourcEditor( cat, widget.viewname->text(), widget.viewname->text(), widget.tooltip->text() );
            break;
        case 1: // Task editor
            m_view->createTaskEditor( cat, widget.viewname->text(), widget.viewname->text(), widget.tooltip->text() );
            break;
        case 2: // Task status
            m_view->createTaskStatusView( cat, widget.viewname->text(), widget.viewname->text(), widget.tooltip->text() );
            break;
        default:
            kError()<<"Unknown view type!";
            break;
    }
    return true;
}

void AddViewPanel::changed()
{
    bool disable = widget.viewname->text().isEmpty() | widget.viewtype->currentText().isEmpty() | widget.category->currentText().isEmpty();
    emit enableButtonOk( ! disable );
}


}  //KPlato namespace

#include "kptviewlistdialog.moc"
