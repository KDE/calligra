/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include <qlayout.h>
#include <qtextedit.h>

#include <kglobalsettings.h>
#include <kdebug.h>

#include <notebar.h>
#include <kpresenter_view.h>
#include <kpresenter_doc.h>
#include <page.h>


NoteBar::NoteBar( QWidget *_parent, KPresenterDoc *_doc, KPresenterView *_view )
    : QWidget( _parent ), doc( _doc ), view( _view )
{
    QBoxLayout *topLayout = new QVBoxLayout( this, 2 );
    textEdit = new QTextEdit( this );

    QFont font = KoGlobal::defaultFont();
    textEdit->setCurrentFont( font );

    int currentPageNum = view->getCurrentPresPage(); // 1 base.
    QString text = doc->getNoteText( currentPageNum - 1 );
    textEdit->setText( text );

    connect( textEdit, SIGNAL( textChanged() ),
             this, SLOT( slotTextChanged() ) );

    topLayout->addWidget( textEdit );
}

NoteBar::~NoteBar()
{
    delete textEdit;
}

void NoteBar::setCurrentNoteText( const QString &_text )
{
    textEdit->setText( _text );
}

void NoteBar::slotTextChanged()
{
    int currentPageNum = view->getCurrPgNum(); // 1 base.
    if ( currentPageNum > 0 )
        doc->setNoteText( currentPageNum - 1, textEdit->text() );
}

#include <notebar.moc>
