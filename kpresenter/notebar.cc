// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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


#include <qlayout.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qlabel.h>

#include <kglobalsettings.h>
#include <kprinter.h>
#include <kdebug.h>
#include <ktextedit.h>
#include <klocale.h>

#include "notebar.h"
#include "kpresenter_view.h"
#include "kpresenter_doc.h"
#include "kprpage.h"


NoteBar::NoteBar( QWidget *_parent, KPresenterView *_view )
    : QWidget( _parent ),
      view( _view ),
      initialize( true )
{
    QBoxLayout *topLayout = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

    label = new QLabel(i18n("Note"), this);

    textEdit = new KTextEdit( this );

    QFont font = KoGlobal::defaultFont();
    textEdit->setCurrentFont( font );

    int currentPageNum = view->getCurrentPresPage(); // 1 base.
    QString text=QString::null;
    if (currentPageNum!=-1)
        text = view->kPresenterDoc()->pageList().at(currentPageNum - 1)->noteText( );
    textEdit->setText( text );

    connect( textEdit, SIGNAL( textChanged() ),
             this, SLOT( slotTextChanged() ) );

    connect( textEdit, SIGNAL( selectionChanged() ),
             this, SLOT( slotSelectionChanged() ) );

    connect( textEdit, SIGNAL( copyAvailable( bool ) ),
             this, SLOT( slotSelectionChanged() ) );

    connect( textEdit, SIGNAL( undoAvailable( bool ) ),
             this, SLOT( slotUndoAvailable( bool ) ) );

    connect( textEdit, SIGNAL( redoAvailable( bool ) ),
             this, SLOT( slotRedoAvailable( bool ) ) );

    topLayout->addWidget( label );
    topLayout->addWidget( textEdit );
}

NoteBar::~NoteBar()
{
    delete textEdit;
}

void NoteBar::setCurrentNoteText( const QString &_text )
{
    initialize = true;
    textEdit->setText( _text );
    initialize = false;
}

void NoteBar::slotTextChanged()
{
    int currentPageNum = view->getCurrPgNum(); // 1 base.
    if ( currentPageNum > 0 && !initialize ) {
        if ( view->editMaster() )
            view->kPresenterDoc()->refreshAllNoteBarMasterPage(textEdit->text() , view);
        else
            view->kPresenterDoc()->refreshAllNoteBar(currentPageNum -1,textEdit->text() , view);
        textEdit->setModified( true );
    }
}

void NoteBar::slotSelectionChanged()
{
    kdDebug(33001) << "slotSelectionChanged(): " << textEdit->hasSelectedText() << endl;
}

void NoteBar::slotCopyAvailable( bool yes )
{
    kdDebug(33001) << "slotCopyAvailable( " << yes << " )" << endl;
}

void NoteBar::slotUndoAvailable( bool /*yes*/ )
{
    //kdDebug(33001) << "slotUndoAvailable( " << yes << " )" << endl;
}

void NoteBar::slotRedoAvailable( bool /*yes*/ )
{
    //kdDebug(33001) << "slotRedoAvailable( " << yes << " )" << endl;
}

void NoteBar::printNotes( QPainter *_painter, KPrinter *_printer, QValueList<int> _list )
{
    // base code from $QTDIR/example/textedit/textedit.cpp
    _painter->save();

    QPaintDeviceMetrics metrics( _painter->device() );
    int dpix = metrics.logicalDpiX();
    int dpiy = metrics.logicalDpiY();

    const int margin = 72; // pt
    QRect body( margin * dpix / 72, margin * dpiy / 72,
                metrics.width() - margin * dpix / 72 * 2,
                metrics.height() - margin * dpiy / 72 * 2 );

    QFont font = KoGlobal::defaultFont();
    QString allText = getNotesTextForPrinting(_list);
    QString str = QStyleSheet::convertFromPlainText( allText );

    QSimpleRichText richText( str, font, QString::null, QStyleSheet::defaultSheet(),
                              QMimeSourceFactory::defaultFactory(), body.height() );

    richText.setWidth( _painter, body.width() );

    QRect viewRect( body );
    do {
        richText.draw( _painter, body.left(), body.top(), viewRect, colorGroup() );
        viewRect.moveBy( 0, body.height() );
        _painter->translate( 0, -body.height() );
        _painter->setFont( font );

        if ( viewRect.top() >= richText.height() )
            break;

        _printer->newPage();
    } while ( true );

    _painter->restore();
}

QString NoteBar::getNotesTextForPrinting(QValueList<int> _list) const
{
    QString allText = QString::null;
    bool firstText = true;
    bool noteIsEmpty = true;
    int pageCount = 1;
    KPresenterDoc *doc=view->kPresenterDoc();
    for ( int i = 0; i < static_cast<int>( doc->pageList().count() ); i++, ++pageCount )
    {
        if (_list.contains(i+1)==0) // that slide isn't printed, don't print its note either
            continue;

        if ( !firstText )
            allText += QString("\n\n");

        allText += i18n( "Slide Note %1:\n" ).arg( pageCount );
        if(noteIsEmpty && !doc->pageList().at(i)->noteText().isEmpty())
            noteIsEmpty = false;
        allText += doc->pageList().at(i)->noteText();

        firstText = false;
    }
    if( noteIsEmpty )
        return QString::null;
    return allText;
}

#include "notebar.moc"
