/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <klocale.h>
#include "kwdoc.h"
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "kweditpath.h"
#include <kfiledialog.h>

KWEditPathDia::KWEditPathDia( const QString & _path, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Edit Path") );
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QGridLayout * grid = new QGridLayout(page, 5, 2, KDialog::marginHint(), KDialog::spacingHint());
    m_listpath = new QListBox( page );
    grid->addMultiCellWidget(m_listpath, 0, 4, 0, 0);
    m_listpath->insertStringList(QStringList::split(QString(";"), _path));
    connect(m_listpath,  SIGNAL( selectionChanged ()), this, SLOT(slotSelectionChanged()));
    m_pbAdd = new QPushButton( i18n("Add path...."), page );
    grid->addWidget( m_pbAdd, 0, 1);
    connect( m_pbAdd, SIGNAL(clicked()), this, SLOT(slotAddPath()));

    m_pbDelete = new QPushButton( i18n("Delete path"), page );
    grid->addWidget( m_pbDelete, 1, 1);

    connect( m_pbDelete, SIGNAL(clicked()), this, SLOT(slotDeletePath()));


    setFocus();
    slotSelectionChanged();
    resize( 300, 200);

}

QString KWEditPathDia::newPath()const
{
    QString tmp;
    for (int i = 0; i <m_listpath->count(); i++)
    {
        if ( i!=0)
            tmp +=";";
        tmp += m_listpath->text( i );
    }
    return tmp;
}

void KWEditPathDia::slotSelectionChanged()
{
    bool state =!m_listpath->currentText().isEmpty();
    enableButtonOK( state );
    m_pbAdd->setEnabled( state);
    m_pbDelete->setEnabled( state );
}

void KWEditPathDia::slotAddPath()
{
    QString path = KFileDialog::getExistingDirectory();
    if ( !path.isEmpty())
    {
        m_listpath->insertItem( path );
    }
}

void KWEditPathDia::slotDeletePath()
{
    m_listpath->removeItem( m_listpath->currentItem ());
}
#include "kweditpath.moc"
