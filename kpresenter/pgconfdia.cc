/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002, 2003 Ariya Hidayat <ariya@kde.org>

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

#include "pgconfdia.h"
#include "kpresenter_doc.h"
#include "kprpage.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpen.h>
#include <qpushbutton.h>
#include <qvaluelist.h>

#include <kcolorbutton.h>
#include <kglobal.h>
#include <klocale.h>
#include <knuminput.h>
#include <qslider.h>

PgConfDia::PgConfDia( QWidget* parent, KPresenterDoc* doc )
    : KDialogBase( KDialogBase::Tabbed, i18n("Configure Slide Show"),
                   Ok|Cancel, Ok, parent, "pgConfDia", true ),
      m_doc( doc )
{
    setupPageGeneral();
    setupPageSlides();

    connect( this, SIGNAL( okClicked() ), this, SLOT( confDiaOk() ) );
    connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
}

void PgConfDia::setupPageGeneral()
{
    QFrame* generalPage = addPage( i18n("&General") );
    QVBoxLayout *generalLayout = new QVBoxLayout( generalPage, KDialog::marginHint(), KDialog::spacingHint() );
    generalLayout->setAutoAdd( true );

    manualSwitch = new QCheckBox( i18n( "&Manual switch to next step" ), generalPage );
    manualSwitch->setChecked( m_doc->spManualSwitch() );
    connect( manualSwitch, SIGNAL( toggled(bool) ), this, SLOT( manualSwitchToggled(bool) ) );

    infiniteLoop = new QCheckBox( i18n( "&Infinite loop" ), generalPage );
    infiniteLoop->setChecked( m_doc->spInfiniteLoop() );
    infiniteLoop->setEnabled( !m_doc->spManualSwitch() );

    presentationDuration = new QCheckBox( i18n( "Show presentation &duration" ), generalPage );
    presentationDuration->setChecked( m_doc->presentationDuration() );

    new QLabel( i18n("Speed:"), generalPage );

    QWidget* sp = new QWidget( generalPage );
    QBoxLayout* speedLayout = new QHBoxLayout( sp, 0, spacingHint() );
    speedLayout->setAutoAdd( true );

    new QLabel( i18n("Slow"), sp );
    speedSlider = new QSlider( 1, 10, 1, 1, Qt::Horizontal, sp );
    speedSlider->setValue( m_doc->getPresSpeed() );
    speedSlider->setTickmarks( QSlider::Below );
    speedSlider->setTickInterval( 1 );
    new QLabel( i18n("Fast"), sp );

    // presentation pen (color and width)

    QGroupBox* penGroup = new QGroupBox( 1, Qt::Horizontal, i18n("Presentation Pen") , generalPage );
    penGroup->setInsideSpacing( KDialog::spacingHint() );
    penGroup->setInsideMargin( KDialog::marginHint() );
    penGroup->setFlat(true);

    new QLabel( i18n( "Color:" ), penGroup );
    penColor = new KColorButton( m_doc->presPen().color(), m_doc->presPen().color(), penGroup );

    new QLabel( i18n( "Width:" ), penGroup );
    penWidth = new KIntNumInput( 1, penGroup );
    penWidth->setSuffix( i18n(" pt") );
    penWidth->setRange( 1, 10, 1 );
    penWidth->setValue( m_doc->presPen().width() );

    QWidget* spacer = new QWidget( generalPage );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
}

void PgConfDia::setupPageSlides()
{
    QFrame* slidesPage = addPage( i18n("&Slides") );
    QVBoxLayout *slidesLayout = new QVBoxLayout( slidesPage, 0, spacingHint() );
    slidesLayout->setAutoAdd( true );

    slides = new QListView( slidesPage );
    slides->addColumn( i18n("Slide") );
    slides->setSorting( -1 );
    slides->header()->hide();

    for ( int i = m_doc->getPageNums() - 1; i >= 0; --i )
    {
        KPrPage *page=m_doc->pageList().at( i );
        QCheckListItem* item = new QCheckListItem( slides,
                                                   page->pageTitle( i18n( "Slide %1" ).arg( i + 1 ) ),
                                                   QCheckListItem::CheckBox );
        item->setOn( page->isSlideSelected() );
    }

    QHBox* buttonGroup = new QHBox( slidesPage );
    buttonGroup->setSpacing( KDialog::spacingHint() );

    QPushButton* selectAllButton = new QPushButton( i18n( "Select &All" ), buttonGroup );
    connect( selectAllButton, SIGNAL( clicked() ), this, SLOT( selectAllSlides() ) );

    QPushButton* deselectAllButton = new QPushButton( i18n( "&Deselect All" ), buttonGroup );
    connect( deselectAllButton, SIGNAL( clicked() ), this, SLOT( deselectAllSlides() ) );

    QWidget* spacer = new QWidget( buttonGroup );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
}

PgConfDia::~PgConfDia()
{
}

bool PgConfDia::getInfiniteLoop() const
{
    return infiniteLoop->isChecked();
}

bool PgConfDia::getManualSwitch() const
{
    return manualSwitch->isChecked();
}

bool PgConfDia::getPresentationDuration() const
{
    return presentationDuration->isChecked();
}

QPen PgConfDia::getPen() const
{
    return QPen( penColor->color(), penWidth->value() );
}

QValueList<bool> PgConfDia::getSelectedSlides() const
{
    QValueList<bool> selectedSlides;

    QListViewItem *item = slides->firstChild();
    while( item )
    {
        QCheckListItem *checkItem = dynamic_cast<QCheckListItem*>( item );
        bool selected = false;
        if( checkItem ) selected = checkItem->isOn();
        item = item->nextSibling();
        selectedSlides.append( selected );
    }
    return selectedSlides;
}

void PgConfDia::selectAllSlides()
{
    QListViewItem *item = slides->firstChild();
    while( item )
    {
        QCheckListItem *checkItem = dynamic_cast<QCheckListItem*>( item );
        if( checkItem ) checkItem->setOn( true );
        item = item->nextSibling();
    }
}

void PgConfDia::deselectAllSlides()
{
    QListViewItem *item = slides->firstChild();
    while( item )
    {
        QCheckListItem *checkItem = dynamic_cast<QCheckListItem*>( item );
        if( checkItem ) checkItem->setOn( false );
        item = item->nextSibling();
    }
}

void PgConfDia::manualSwitchToggled( bool state )
{
    infiniteLoop->setEnabled( !state );
}

PresSpeed PgConfDia::getPresSpeed() const
{
    int value = speedSlider->value();
    if( value <= 0 ) value = 1;
    return static_cast<PresSpeed>(value);
}

#include "pgconfdia.moc"
