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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include "KWDocument.h"
#include <q3vbox.h>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <kmessagebox.h>
#include "KWCommand.h"
#include "KWCreateBookmarkDia.h"
#include "KWCreateBookmarkDiaBase.h"
#include "KWFrame.h"
#include "KWFrameSet.h"

KWCreateBookmarkDia::KWCreateBookmarkDia( const QStringList & _list, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Create New Bookmark") );
    listBookMark = _list;
    init();
}

KWCreateBookmarkDia::KWCreateBookmarkDia( const QStringList & _list, const QString & _name, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Rename Bookmark...") );
    listBookMark = _list;
    init();
    m_bookmarkName->setText(_name);
}

void KWCreateBookmarkDia::init()
{
    KWCreateBookmarkDiaBase *dia = new KWCreateBookmarkDiaBase( this );
    m_bookmarkName = dia->m_bookmarkName;
    enableButtonOK( false );
    connect( m_bookmarkName, SIGNAL(textChanged ( const QString & )), this, SLOT(nameChanged( const QString &)));
    setMainWidget( dia );
    m_bookmarkName->setFocus();
}

void KWCreateBookmarkDia::slotOk()
{
    if ( listBookMark.findIndex(m_bookmarkName->text() ) != -1 )
    {
        KMessageBox::error(this, i18n("That name already exists, please choose another name."));
    }
    else
        KDialogBase::slotOk();
}

QString KWCreateBookmarkDia::bookmarkName()const
{
    return m_bookmarkName->text();
}

void KWCreateBookmarkDia::nameChanged( const QString &text)
{
    enableButtonOK( !text.isEmpty() );
}


/* ****************************  */
KWSelectBookmarkDia::KWSelectBookmarkDia( const QStringList & _list, KWDocument *_doc, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    m_doc=_doc;
    setCaption( i18n("Select Bookmark") );
    QWidget *page = new QWidget( this );
    setMainWidget(page);

    Q3GridLayout * grid = new Q3GridLayout(page, 5, 2, KDialog::marginHint(), KDialog::spacingHint());
    m_bookmarkList = new Q3ListBox( page );
    grid->addMultiCellWidget(m_bookmarkList, 0, 4, 0, 0);
    m_bookmarkList->insertStringList(_list);

    connect(m_bookmarkList,  SIGNAL( selectionChanged ()), this, SLOT(slotSelectionChanged()));
    connect(m_bookmarkList,  SIGNAL(doubleClicked ( Q3ListBoxItem * )), this, SLOT(slotOk()));
    connect(m_bookmarkList,  SIGNAL(returnPressed ( Q3ListBoxItem * )), this, SLOT(slotOk()));

    m_pbRename = new QPushButton( i18n("Rename Bookmark"), page );
    grid->addWidget( m_pbRename, 0, 1);
    connect( m_pbRename, SIGNAL(clicked()), this, SLOT(slotRenameBookmark()));

    m_pbDelete = new QPushButton( i18n("Delete Bookmark"), page );
    grid->addWidget( m_pbDelete, 1, 1);

    connect( m_pbDelete, SIGNAL(clicked()), this, SLOT(slotDeleteBookmark()));

    m_bookmarkList->setFocus();
    slotSelectionChanged();
}

void KWSelectBookmarkDia::slotRenameBookmark()
{
    QString tmp =m_bookmarkList->currentText();
    if ( tmp.isEmpty() )
        return;
    //all bookmark name
    QStringList lst =m_doc->listOfBookmarkName(0L);
    lst.remove( tmp );
    KWCreateBookmarkDia dia( lst, tmp, this, 0 );
    if ( dia.exec() ) {
        QString newName = dia.bookmarkName();
        KWRenameBookmarkCommand *cmd = new KWRenameBookmarkCommand( i18n("Rename Bookmark"), tmp, newName, m_doc);
        m_doc->addCommand( cmd );
        cmd->execute();
        m_bookmarkList->changeItem ( newName, m_bookmarkList->currentItem() );
    }
}

void KWSelectBookmarkDia::slotDeleteBookmark()
{
    QString tmp =m_bookmarkList->currentText();
    if ( !tmp.isEmpty())
    {
        m_doc->deleteBookmark(tmp);
        m_bookmarkList->removeItem(m_bookmarkList->currentItem());
    }
}


QString KWSelectBookmarkDia::bookmarkSelected()const
{
    return m_bookmarkList->currentText();
}

void KWSelectBookmarkDia::slotSelectionChanged()
{
    bool state =!m_bookmarkList->currentText().isEmpty();
    enableButtonOK( state );
    m_pbRename->setEnabled( state);
    m_pbDelete->setEnabled( state && m_doc->isReadWrite());
}

#include "KWCreateBookmarkDia.moc"
