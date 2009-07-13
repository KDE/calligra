/* This file is part of the KDE project
   Copyright (C) 20079 Dag Andersen <danders@get2net.dk>

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

#include "kptinsertfiledlg.h"
#include "kptnode.h"
#include "kptproject.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include <QRadioButton>

namespace KPlato
{

// little helper stolen from kmail/kword
static inline QPixmap loadIcon( const char * name ) {
  return KIconLoader::global()->loadIcon( QString::fromLatin1(name), KIconLoader::NoGroup, KIconLoader::SizeMedium );
}


InsertFileDialog::InsertFileDialog( Project &project, Node *currentNode, QWidget *parent )
    : KDialog(parent)
{
    setCaption( i18n("Insert File") );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setDefaultButton( Ok );

    m_panel = new InsertFilePanel( project, currentNode, this );

    setMainWidget( m_panel );
    
    enableButtonOk(false);

    connect( m_panel, SIGNAL( enableButtonOk( bool ) ), SLOT( enableButtonOk( bool ) ) );
}

KUrl InsertFileDialog::url() const
{
    return m_panel->url();
}

Node *InsertFileDialog::parentNode() const
{
    return m_panel->parentNode();
}

Node *InsertFileDialog::afterNode() const
{
    return m_panel->afterNode();
}

//------------------------
InsertFilePanel::InsertFilePanel( Project &project, Node *currentNode, QWidget *parent )
    : QWidget( parent ),
    m_project( project ),
    m_node( currentNode )
{
    ui.setupUi( this );
    
    if ( currentNode == 0 || currentNode->type() == Node::Type_Project ) {
        ui.ui_isAfter->setEnabled( false );
        ui.ui_isParent->setEnabled( false );
        ui.ui_useProject->setChecked( true );

        ui.ui_name->setText( project.name() );
    } else {
        ui.ui_isAfter->setChecked( true );

        ui.ui_name->setText( currentNode->name() );
    }
    connect( ui.ui_url, SIGNAL( textChanged( const QString& ) ), SLOT( changed( const QString& ) ) );

    connect( ui.ui_url, SIGNAL( openFileDialog( KUrlRequester* ) ), SLOT( slotOpenFileDialog( KUrlRequester* ) ) );
}

void InsertFilePanel::slotOpenFileDialog( KUrlRequester * )
{
    ui.ui_url->setFilter( "*.kplato" );
}

void InsertFilePanel::changed( const QString &text )
{
    emit enableButtonOk( ! text.isEmpty() );
}

KUrl InsertFilePanel::url() const
{
    return ui.ui_url->url();
}

Node *InsertFilePanel::parentNode() const
{
    if ( ui.ui_useProject->isChecked() ) {
        return &(m_project);
    }
    if ( ui.ui_isParent->isChecked() ) {
        return m_node;
    }
    if ( ui.ui_isAfter->isChecked() ) {
        return m_node->parentNode();
    }
    return &(m_project);
}

Node *InsertFilePanel::afterNode() const
{
    if ( ui.ui_isAfter->isChecked() ) {
        return m_node;
    }
    return 0;
}


}  //KPlato namespace

#include "kptinsertfiledlg.moc"
