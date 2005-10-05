/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/


/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include <qcolordialog.h>
void itemAttributeDialog::init( )
{

    myItem = 0;
}




void itemAttributeDialog::ChangeText_clicked()
{
    if ( !myItem) return;
    QColor c = QColorDialog::getColor( myItem->textColor(), this );
    if ( c.isValid() )
      myItem->setTextColor( c );
    QPixmap *  pix  = (QPixmap *)ChangeText->pixmap();
    pix->fill( myItem->textColor() );
    ChangeText->repaint();
}


void itemAttributeDialog::ChangeStart_clicked()
{
    if ( !myItem) return;
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->colors( st, mi, en );
    QColor c = QColorDialog::getColor( st, this );
    if ( c.isValid() ) {
	st = c;
	ChangeStart->setPixmap(  KDGanttView::getPixmap( start, st, backgroundColor(), 10 )  );
	myItem->setColors( st, mi, en );
    }

}


void itemAttributeDialog::ChangeMiddle_clicked()
{
    if ( !myItem) return;
     KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->colors( st, mi, en );
    QColor c = QColorDialog::getColor( mi, this );
    if ( c.isValid() ) {
	mi = c;
	ChangeMiddle->setPixmap(  KDGanttView::getPixmap( middle, mi, backgroundColor(), 10 )  );
	myItem->setColors( st, mi, en );
    }
}


void itemAttributeDialog::ChangeEnd_clicked()
{
    if ( !myItem) return;
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->colors( st, mi, en );
     QColor c = QColorDialog::getColor( en, this );
    if ( c.isValid() ) {
	en = c;
	ChangeEnd->setPixmap(  KDGanttView::getPixmap( end, en, backgroundColor(), 10 )  );
	myItem->setColors( st, mi, en );
    }
}
void itemAttributeDialog::resetTime( KDGanttViewItem * item )
{
    if ( !item ) return;
    myItem = 0;
    DateEdit1->setDate( item->startTime().date() );
    TimeEdit1->setTime( item->startTime().time() );
    switch( item->type() ) {
	case KDGanttViewItem::Event:

	DateEdit4->setDate( ((KDGanttViewEventItem*)item)->leadTime().date() );
	TimeEdit4->setTime( ((KDGanttViewEventItem*)item)->leadTime().time() );

	break;
          case KDGanttViewItem::Summary:


	  DateEdit3->setDate( item->endTime().date() );
	  TimeEdit3->setTime( item->endTime().time() );
	  DateEdit2->setDate(((KDGanttViewSummaryItem*)item) ->middleTime().date() );
	  TimeEdit2->setTime( ((KDGanttViewSummaryItem*)item)->middleTime().time() );
	  DateEdit5->setDate(((KDGanttViewSummaryItem*)item) ->actualEndTime().date() );
	  TimeEdit5->setTime( ((KDGanttViewSummaryItem*)item)->actualEndTime().time() );
	  break;
          case KDGanttViewItem::Task:


	  DateEdit3->setDate( item->endTime().date() );
	  TimeEdit3->setTime( item->endTime().time() );
	  break;
	default:
	  ;
	}
     myItem = item;
}

void itemAttributeDialog::reset( KDGanttViewItem * item )
{ 
    myItem = 0;
    if ( !item ) {
	hide();
	return;
    }
    StartBox->setEnabled( true );
    switch( item->type() ) {
         case KDGanttViewItem::Event:
	 MiddleBox->setEnabled( false );
	 EndBox->setEnabled( false );
	 ChangeMiddle->setEnabled( false );
	 ChangeEnd->setEnabled( false );
             HighMiddle->setEnabled( false );
	 HighEnd->setEnabled( false );
	  DateEdit2->setEnabled( false );
	  TimeEdit2->setEnabled( false );
	  DateEdit3->setEnabled( false );
	  TimeEdit3->setEnabled( false );
	  DateEdit4->setEnabled( true );
	  TimeEdit4->setEnabled( true );
	  DateEdit5->setEnabled( false );
	  TimeEdit5->setEnabled( false );
	   DateEdit4->setDate( ((KDGanttViewEventItem*)item)->leadTime().date() );
	  TimeEdit4->setTime( ((KDGanttViewEventItem*)item)->leadTime().time() );

	break;
          case KDGanttViewItem::Summary:
	 MiddleBox->setEnabled( true );
	 EndBox->setEnabled( true );
	 ChangeMiddle->setEnabled( true );
	 ChangeEnd->setEnabled( true );
             HighMiddle->setEnabled( true );
	 HighEnd->setEnabled( true );

	  DateEdit2->setEnabled( true );
	  TimeEdit2->setEnabled( true );
	  DateEdit3->setEnabled( true );
	  TimeEdit3->setEnabled( true );
	  DateEdit4->setEnabled( false );
	  TimeEdit4->setEnabled( false );
	  DateEdit5->setEnabled( true );
	  TimeEdit5->setEnabled( true );

	  DateEdit3->setDate( item->endTime().date() );
	  TimeEdit3->setTime( item->endTime().time() );
	  DateEdit2->setDate(((KDGanttViewSummaryItem*)item) ->middleTime().date() );
	  TimeEdit2->setTime( ((KDGanttViewSummaryItem*)item)->middleTime().time() );
	  DateEdit5->setDate(((KDGanttViewSummaryItem*)item) ->actualEndTime().date() );
	  TimeEdit5->setTime( ((KDGanttViewSummaryItem*)item)->actualEndTime().time() );
	  break;
          case KDGanttViewItem::Task:
	 MiddleBox->setEnabled( false );
	 StartBox->setEnabled( false );
	 EndBox->setEnabled( false );
	 ChangeMiddle->setEnabled( false );
	 ChangeEnd->setEnabled( false );
             HighMiddle->setEnabled( false );
	 HighEnd->setEnabled( false );

	  DateEdit2->setEnabled( false );
	  TimeEdit2->setEnabled( false );
	  DateEdit3->setEnabled( true );
	  TimeEdit3->setEnabled( true );
	  DateEdit4->setEnabled( false );
	  TimeEdit4->setEnabled( false );
	  DateEdit5->setEnabled( false );
	  TimeEdit5->setEnabled( false );

	  DateEdit3->setDate( item->endTime().date() );
	  TimeEdit3->setTime( item->endTime().time() );
	  break;
	default:
	  ;
	}
    if (item->firstChild() && item->displaySubitemsAsGroup() ) {
	
	  DateEdit2->setEnabled( false );
	  TimeEdit2->setEnabled( false );
	  DateEdit3->setEnabled( false );
	  TimeEdit3->setEnabled( false );
	  DateEdit4->setEnabled( false );
	  TimeEdit4->setEnabled( false );
	  DateEdit5->setEnabled( false );
	  TimeEdit5->setEnabled( false );
	  DateEdit1->setEnabled( false );
	  TimeEdit1->setEnabled( false );
	
      } else {
	  DateEdit1->setEnabled( true );
	  TimeEdit1->setEnabled( true );    
      }
     DateEdit1->setDate( item->startTime().date() );
    TimeEdit1->setTime( item->startTime().time() );
    if ( item->pixmap() != 0 )	
	setIcon( *(item->pixmap()) );
    setCaption( "Properties of " + ((QListViewItem*)item)->text(0) );
    itemName->setText(((QListViewItem*)item)->text(0) );

//    DateEdit1->setRange(item->startTime().date().addYears(-10), item->endTime().date() );
  //  DateEdit3->setRange(item->startTime().date(), item->endTime().date().addYears(10));
    LineEdit1->setText( item->text() );
    KDGanttViewItem::Shape start,  middle, end;
    item->shapes( start, middle, end );
    QColor st, mi, en;
    item->colors( st, mi, en );
    ChangeStart->setPixmap(  KDGanttView::getPixmap( start, st, backgroundColor(), 10 )  );
    ChangeMiddle->setPixmap(  KDGanttView::getPixmap( middle, mi, backgroundColor(), 10 )  );
    ChangeEnd->setPixmap(  KDGanttView::getPixmap( end, en, backgroundColor(), 10 )  );
    item->highlightColors( st, mi, en );
    HighStart->setPixmap(  KDGanttView::getPixmap( start, st, backgroundColor(), 10 )  );
    HighMiddle->setPixmap(  KDGanttView::getPixmap( middle, mi, backgroundColor(), 10 )  );
    HighEnd->setPixmap(  KDGanttView::getPixmap( end, en, backgroundColor(), 10 )  );
    ChangeText->setPixmap(QPixmap( 16,16 ));
    QPixmap * pix;
    pix  = (QPixmap *)ChangeText->pixmap();
    pix->fill( item->textColor() );
    StartBox->setCurrentItem((int)start );
    MiddleBox->setCurrentItem((int)middle );
    EndBox->setCurrentItem( (int) end );
    CalBox->setChecked( item-> displaySubitemsAsGroup() );
    PrioSpinBox->setValue( item->priority() );
    PrioSlider->setValue( item->priority() );
    myItem = item;

}

void itemAttributeDialog::HighStart_clicked()
{
    if ( !myItem) return;
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->highlightColors( st, mi, en );
    QColor c = QColorDialog::getColor( st, this );
    if ( c.isValid() ) {
	st = c;
	HighStart->setPixmap(  KDGanttView::getPixmap( start, st, backgroundColor(), 10 )  );
	myItem->setHighlightColors( st, mi, en );
    }

}


void itemAttributeDialog::HighMiddle_clicked()
{
    if ( !myItem) return;
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->highlightColors( st, mi, en );
    QColor c = QColorDialog::getColor( mi, this );
    if ( c.isValid() ) {
	mi = c;
	HighMiddle->setPixmap(  KDGanttView::getPixmap( middle, mi, backgroundColor(), 10 )  );
	myItem->setHighlightColors( st, mi, en );
    }

}


void itemAttributeDialog::HighEnd_clicked()
{
    if ( !myItem) return;
   KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    QColor st, mi, en;
    myItem->highlightColors( st, mi, en );
    QColor c = QColorDialog::getColor( en, this );
    if ( c.isValid() ) {
	en = c;
	HighEnd->setPixmap(  KDGanttView::getPixmap( end, en, backgroundColor(), 10 )  );
	myItem->setHighlightColors( st, mi, en );
    }

}


void itemAttributeDialog::DateEdit1_valueChanged( const QDate & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit1->date() );
    dt.setTime( TimeEdit1->time() );
    if ( dt.isValid() )
    {
        myItem->setStartTime( dt );
    resetTime( myItem );
   }
}


void itemAttributeDialog::TimeEdit1_valueChanged( const QTime & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit1->date() );
    dt.setTime( TimeEdit1->time() );
    if ( dt.isValid() ) {
    myItem->setStartTime( dt );
   
    resetTime( myItem );
}
}


void itemAttributeDialog::DateEdit2_valueChanged( const QDate & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit2->date() );
    dt.setTime( TimeEdit2->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewSummaryItem*)myItem)->setMiddleTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::TimeEdit2_valueChanged( const QTime & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit2->date() );
    dt.setTime( TimeEdit2->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewSummaryItem*)myItem)->setMiddleTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::DateEdit3_valueChanged( const QDate & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit3->date() );
    dt.setTime( TimeEdit3->time() );
    if ( dt.isValid() ) {
    myItem->setEndTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::TimeEdit3_valueChanged( const QTime & )
{
    if ( !myItem) return;
   QDateTime dt;
    dt.setDate( DateEdit3->date() );
    dt.setTime( TimeEdit3->time() );
    if ( dt.isValid() ) {
	myItem->setEndTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::LineEdit1_textChanged( const QString & )
{
    if ( !myItem) return;
    myItem->setText(LineEdit1->text());
}


void itemAttributeDialog::StartBox_activated( const QString & s )
{
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    start = KDGanttViewItem::stringToShape( s );
    myItem->setShapes( start, middle, end );
    reset( myItem );
}


void itemAttributeDialog::MiddleBox_activated( const QString & s )
{
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    middle = KDGanttViewItem::stringToShape( s );
    myItem->setShapes( start, middle, end );
    reset( myItem );

}


void itemAttributeDialog::EndBox_activated( const QString & s )
{
    KDGanttViewItem::Shape start,  middle, end;
    myItem->shapes( start, middle, end );
    end = KDGanttViewItem::stringToShape( s );
    myItem->setShapes( start, middle, end );
    reset( myItem );

}


void itemAttributeDialog::DateEdit4_valueChanged( const QDate & )
{
   if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit4->date() );
    dt.setTime( TimeEdit4->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewEventItem*)myItem)->setLeadTime( dt );
    resetTime( myItem );
    }
}


void itemAttributeDialog::TimeEdit4_valueChanged( const QTime & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit4->date() );
    dt.setTime( TimeEdit4->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewEventItem*)myItem)->setLeadTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::DateEdit5_valueChanged( const QDate & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit5->date() );
    dt.setTime( TimeEdit5->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewSummaryItem*)myItem)->setActualEndTime( dt );
    resetTime( myItem );
}
}


void itemAttributeDialog::TimeEdit5_valueChanged( const QTime & )
{
    if ( !myItem) return;
    QDateTime dt;
    dt.setDate( DateEdit5->date() );
    dt.setTime( TimeEdit5->time() );
    if ( dt.isValid() ) {
    ((KDGanttViewSummaryItem*)myItem)->setActualEndTime( dt );
    resetTime( myItem );
}
}



void itemAttributeDialog::itemName_textChanged( const QString & )
{
    if ( !myItem) return;
    ((QListViewItem*)myItem)->setText( 0, itemName->text() );
    setCaption( "Properties of " + itemName->text() );
}


void itemAttributeDialog::PrioSpinBox_valueChanged( int val )
{
   if ( !myItem) return;
   myItem->setPriority( val );
   PrioSlider->blockSignals( true );
   PrioSlider->setValue( val );
   PrioSlider->blockSignals( false );
   
}


void itemAttributeDialog::CalBox_toggled( bool mode )
{
    if ( !myItem) return;
    myItem->setDisplaySubitemsAsGroup( mode );
    if (myItem->firstChild() )
    reset(myItem);
}


void itemAttributeDialog::PrioSlider_valueChanged( int val )
{
if ( !myItem) return;
   myItem->setPriority( val );
   PrioSpinBox->blockSignals( true );
   PrioSpinBox->setValue( val );
   PrioSpinBox->blockSignals( false );
}


KDGanttViewItem* itemAttributeDialog::getItem()
{
 return myItem;
}
