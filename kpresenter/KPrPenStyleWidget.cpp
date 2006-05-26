// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004-2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrPenStyleWidget.h"

#include "penstyle.h"
#include "KPrPBPreview.h"

#include <QLayout>
#include <QLabel>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>


KPrPenStyleWidget::KPrPenStyleWidget( QWidget *parent, const char *name, const KoPenCmd::Pen &pen, bool configureLineEnds )
: QWidget( parent, name )
, m_pen( pen )
{
    Q3VBoxLayout *layout = new Q3VBoxLayout( this );
    layout->addWidget( m_ui = new PenStyleUI( this ) );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addItem( spacer );

    connect( m_ui->colorChooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotPenChanged() ) );

    m_ui->styleCombo->insertItem( i18n( "No Outline" ) );
    m_ui->styleCombo->insertItem( "__________" );
    m_ui->styleCombo->insertItem( "__ __ __ __" );
    m_ui->styleCombo->insertItem( "_ _ _ _ _ _" );
    m_ui->styleCombo->insertItem( "__ _ __ _ __" );
    m_ui->styleCombo->insertItem( "__ _ _ __ _" );

    m_ui->widthInput->setRange( 1, 10,  1, false);

    connect( m_ui->styleCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotPenChanged() ) );

    connect( m_ui->widthInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotPenChanged() ) );

    m_ui->lineBeginCombo->insertItem( i18n("Normal") );
    m_ui->lineBeginCombo->insertItem( i18n("Arrow") );
    m_ui->lineBeginCombo->insertItem( i18n("Square") );
    m_ui->lineBeginCombo->insertItem( i18n("Circle") );
    m_ui->lineBeginCombo->insertItem( i18n("Line Arrow") );
    m_ui->lineBeginCombo->insertItem( i18n("Dimension Line") );
    m_ui->lineBeginCombo->insertItem( i18n("Double Arrow") );
    m_ui->lineBeginCombo->insertItem( i18n("Double Line Arrow") );

    connect( m_ui->lineBeginCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotLineBeginChanged() ) );

    m_ui->lineEndCombo->insertItem( i18n("Normal") );
    m_ui->lineEndCombo->insertItem( i18n("Arrow") );
    m_ui->lineEndCombo->insertItem( i18n("Square") );
    m_ui->lineEndCombo->insertItem( i18n("Circle") );
    m_ui->lineEndCombo->insertItem( i18n("Line Arrow") );
    m_ui->lineEndCombo->insertItem( i18n("Dimension Line") );
    m_ui->lineEndCombo->insertItem( i18n("Double Arrow") );
    m_ui->lineEndCombo->insertItem( i18n("Double Line Arrow") );

    connect( m_ui->lineEndCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotLineEndChanged() ) );

    if ( !configureLineEnds )
        m_ui->arrowGroup->hide();
    //m_ui->arrowGroup->setEnabled( configureLineEnds );

    slotReset();
}


KPrPenStyleWidget::~KPrPenStyleWidget()
{
    delete m_ui;
}


void KPrPenStyleWidget::setPen( const KoPen &pen )
{
    m_ui->colorChooser->setColor( pen.color() );

    switch ( pen.style() )
    {
        case NoPen:
            m_ui->styleCombo->setCurrentItem( 0 );
            break;
        case SolidLine:
            m_ui->styleCombo->setCurrentItem( 1 );
            break;
        case DashLine:
            m_ui->styleCombo->setCurrentItem( 2 );
            break;
        case DotLine:
            m_ui->styleCombo->setCurrentItem( 3 );
            break;
        case DashDotLine:
            m_ui->styleCombo->setCurrentItem( 4 );
            break;
        case DashDotDotLine:
            m_ui->styleCombo->setCurrentItem( 5 );
            break;
        case MPenStyle:
            break; // not supported.
    }

    m_ui->widthInput->setValue( pen.pointWidth() );
    m_ui->pbPreview->setPen( pen );
}


void KPrPenStyleWidget::setLineBegin( LineEnd lb )
{
    m_ui->lineBeginCombo->setCurrentItem( (int)lb );
    m_ui->pbPreview->setLineBegin( lb );
}


void KPrPenStyleWidget::setLineEnd( LineEnd le )
{
    m_ui->lineEndCombo->setCurrentItem( (int)le );
    m_ui->pbPreview->setLineEnd( le );
}


KoPen KPrPenStyleWidget::getKPPen() const
{
    KoPen pen;

    switch ( m_ui->styleCombo->currentItem() )
    {
        case 0:
            pen.setStyle( NoPen );
            break;
        case 1:
            pen.setStyle( SolidLine );
            break;
        case 2:
            pen.setStyle( DashLine );
            break;
        case 3:
            pen.setStyle( DotLine );
            break;
        case 4:
            pen.setStyle( DashDotLine );
            break;
        case 5:
            pen.setStyle( DashDotDotLine );
            break;
    }

    pen.setColor( m_ui->colorChooser->color() );
    pen.setPointWidth( m_ui->widthInput->value() );

    return pen;
}


LineEnd KPrPenStyleWidget::getLineBegin() const
{
    return (LineEnd) m_ui->lineBeginCombo->currentItem();
}


LineEnd KPrPenStyleWidget::getLineEnd() const
{
    return (LineEnd) m_ui->lineEndCombo->currentItem();
}


int KPrPenStyleWidget::getPenConfigChange() const
{
    int flags = 0;

    if ( getLineEnd() != m_pen.lineEnd )
        flags = flags | KoPenCmd::LineEnd;
    if ( getLineBegin() != m_pen.lineBegin )
        flags = flags | KoPenCmd::LineBegin;
    if ( getKPPen().color() != m_pen.pen.color() )
        flags = flags | KoPenCmd::Color;
    if ( getKPPen().style() != m_pen.pen.style() )
        flags = flags | KoPenCmd::Style;
    if ( getKPPen().pointWidth() != m_pen.pen.pointWidth() )
        flags = flags | KoPenCmd::Width;

    return flags;
}


KoPenCmd::Pen KPrPenStyleWidget::getPen() const
{
    KoPenCmd::Pen pen( getKPPen(), getLineBegin(), getLineEnd() );
    return pen;
}


void KPrPenStyleWidget::setPen( const KoPenCmd::Pen &pen )
{
    m_pen = pen;
    slotReset();
}


void KPrPenStyleWidget::apply()
{
    int flags = getPenConfigChange();

    if ( flags & KoPenCmd::LineEnd )
        m_pen.lineEnd = getLineEnd();

    if ( flags & KoPenCmd::LineBegin )
        m_pen.lineBegin = getLineBegin();

    if ( flags & KoPenCmd::Color )
        m_pen.pen.setColor( getKPPen().color() );

    if ( flags & KoPenCmd::Style )
        m_pen.pen.setStyle( getKPPen().style() );

    if ( flags & KoPenCmd::Width )
        m_pen.pen.setPointWidth( getKPPen().pointWidth() );
}


void KPrPenStyleWidget::slotReset()
{
    setPen( m_pen.pen );
    m_ui->widthLabel->setEnabled( m_pen.pen.style() != NoPen );
    m_ui->widthInput->setEnabled( m_pen.pen.style() != NoPen );

    setLineBegin( m_pen.lineBegin );
    setLineEnd( m_pen.lineEnd );
}


void KPrPenStyleWidget::slotPenChanged()
{
    KoPen pen = getKPPen();
    m_ui->widthLabel->setEnabled( pen.style() != NoPen );
    m_ui->widthInput->setEnabled( pen.style() != NoPen );
    m_ui->pbPreview->setPen( pen );
}


void KPrPenStyleWidget::slotLineBeginChanged()
{
    m_ui->pbPreview->setLineBegin( getLineBegin() );
}


void KPrPenStyleWidget::slotLineEndChanged()
{
    m_ui->pbPreview->setLineEnd( getLineEnd() );
}

#include "KPrPenStyleWidget.moc"
