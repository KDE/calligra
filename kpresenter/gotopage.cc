/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "gotopage.h"
#include "gotopage.moc"

#include <qlabel.h>
#include <qcombobox.h>
#include <qvalidator.h>
#include <qevent.h>

#include <kapp.h>
#include <klocale.h>

/******************************************************************/
/* class KPGotoPage						  */
/******************************************************************/

/*================================================================*/
KPGotoPage::KPGotoPage( const QValueList<int> &slides, int start,
			QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f ), _default( start ), page( start )
{
    label = new QLabel( i18n( "Goto Page:" ), this );
    label->resize( label->sizeHint() );

    spinbox = new QComboBox( false, this );
    spinbox->resize( spinbox->sizeHint() );

    QValueList<int>::ConstIterator it = slides.begin();
    unsigned int i = 0;
    for ( unsigned int j = 0; it != slides.end(); ++it, ++j )
    {
	spinbox->insertItem( QString( "%1" ).arg( *it ), -1 );
	if ( *it == start )
	    i = j;
    }
    spinbox->setCurrentItem( i );

    spinbox->installEventFilter( this );
    label->installEventFilter( this );

    setFrameStyle( QFrame::WinPanel | QFrame:: Raised );

    spinbox->grabMouse();
    spinbox->grabKeyboard();
    setFocusProxy( spinbox );
    setFocusPolicy( QWidget::StrongFocus );
    spinbox->setFocus();

    resize( spinbox->width() + label->width() + 16,
	    QMAX( spinbox->height(), label->height() ) + 10 );

    move( ( kapp->desktop()->width() - width() ) / 2,
	  ( kapp->desktop()->height() - height() ) / 2 );

    show();
}

/*================================================================*/
int KPGotoPage::gotoPage( const QValueList<int> &slides, int start, QWidget *parent)
{
    KPGotoPage dia( slides, start,parent, 0L,
		    Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WType_Popup );

    kapp->enter_loop();

    int page = dia.getPage();

    return page;
}

/*================================================================*/
bool KPGotoPage::eventFilter( QObject * /*obj*/, QEvent *e )
{
#undef KeyPress

    if ( e->type() == QEvent::KeyPress )
    {
	QKeyEvent *ke = dynamic_cast<QKeyEvent*>( e );
	if ( ke->key() == Key_Enter || ke->key() == Key_Return )
	{
	    page = spinbox->currentText().toInt();

	    spinbox->releaseMouse();
	    spinbox->releaseKeyboard();

	    kapp->exit_loop();
	    hide();

	    return true;
	}
	else if ( ke->key() == Key_Escape )
	{
	    page = _default;

	    spinbox->releaseMouse();
	    spinbox->releaseKeyboard();

	    kapp->exit_loop();
	    hide();

	    return true;
	}
	else if ( ke->key() == Key_Down )
	{
	    if ( spinbox->currentItem() < spinbox->count() )
	    {
		spinbox->setCurrentItem( spinbox->currentItem() + 1 );
		page = spinbox->currentText().toInt();
	    }
	    return true;
	}
	else if ( ke->key() == Key_Up )
	{
	    if ( spinbox->currentItem() > 0 )
	    {
		spinbox->setCurrentItem( spinbox->currentItem() - 1 );
		page = spinbox->currentText().toInt();
	    }
	    return true;
	}
    }
    return false;
}

/*================================================================*/
void KPGotoPage::resizeEvent( QResizeEvent * /*e*/ )
{
    spinbox->resize( spinbox->sizeHint() );
    label->resize( label->sizeHint() );
    label->resize( label->width(), QMAX( label->height(), spinbox->height() ) );

    label->move( 5, 5 );
    spinbox->move( label->x() + label->width() + 5, 5 );
}

