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

#include <qvbox.h>
#include <qlineedit.h>
#include <kmessagebox.h>
#include "kwcreatebookmarkdia.h"

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
    setCaption( i18n("Rename Bookmark") );
    listBookMark = _list;
    init();
    m_bookmarkName->setText(_name);
}

void KWCreateBookmarkDia::init()
{
    QVBox *page = makeVBoxMainWidget();
    m_bookmarkName = new QLineEdit( page );
    connect( m_bookmarkName, SIGNAL(textChanged ( const QString & )), this, SLOT(nameChanged( const QString &)));
    m_bookmarkName->setFocus();
    enableButtonOK( false );
}

void KWCreateBookmarkDia::slotOk()
{
    if ( listBookMark.findIndex(m_bookmarkName->text() ) != -1 )
    {
        KMessageBox::error(this, i18n("Name already exists! Please choose another name"));
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

#include "kwcreatebookmarkdia.moc"
