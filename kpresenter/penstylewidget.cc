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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "penstylewidget.h"

#include "penstyle.h"
#include "pbpreview.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>


PenStyleWidget::PenStyleWidget( QWidget *parent, const char *name, const PenCmd::Pen &pen, bool configureLineEnds )
: QWidget( parent, name )
, m_pen( pen )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( m_ui = new PenStyleUI( this ) );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addItem( spacer );

    connect( m_ui->colorChooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotPenChanged() ) );

    m_ui->styleCombo->insertItem( i18n( "No Outline" ) );
    m_ui->styleCombo->insertItem( i18n( "Solid Line" ) );
    m_ui->styleCombo->insertItem( i18n( "Dash Line ( ---- )" ) );
    m_ui->styleCombo->insertItem( i18n( "Dot Line ( **** )" ) );
    m_ui->styleCombo->insertItem( i18n( "Dash Dot Line ( -*-* )" ) );
    m_ui->styleCombo->insertItem( i18n( "Dash Dot Dot Line ( -**- )" ) );

    connect( m_ui->styleCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotPenChanged() ) );

    connect( m_ui->widthInput, SIGNAL( valueChanged( int ) ),
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


PenStyleWidget::~PenStyleWidget()
{
    delete m_ui;
}


void PenStyleWidget::setPen( const QPen &pen )
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

    m_ui->widthInput->setValue( pen.width() );
    m_ui->pbPreview->setPen( pen );
}


void PenStyleWidget::setLineBegin( LineEnd lb )
{
    m_ui->lineBeginCombo->setCurrentItem( (int)lb );
    m_ui->pbPreview->setLineBegin( lb );
}


void PenStyleWidget::setLineEnd( LineEnd le )
{
    m_ui->lineEndCombo->setCurrentItem( (int)le );
    m_ui->pbPreview->setLineEnd( le );
}


QPen PenStyleWidget::getPen() const
{
    QPen pen;

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
    pen.setWidth( m_ui->widthInput->value() );

    return pen;
}


LineEnd PenStyleWidget::getLineBegin() const
{
    return (LineEnd) m_ui->lineBeginCombo->currentItem();
}


LineEnd PenStyleWidget::getLineEnd() const
{
    return (LineEnd) m_ui->lineEndCombo->currentItem();
}


int PenStyleWidget::getPenConfigChange() const
{
    int flags = 0;

    if ( getLineEnd() != m_pen.lineEnd )
        flags = flags | PenCmd::LineEnd;
    if ( getLineBegin() != m_pen.lineBegin )
        flags = flags | PenCmd::LineBegin;
    if ( getPen().color() != m_pen.pen.color() )
        flags = flags | PenCmd::Color;
    if ( getPen().style() != m_pen.pen.style() )
        flags = flags | PenCmd::Style;
    if ( getPen().width() != m_pen.pen.width() )
        flags = flags | PenCmd::Width;

    return flags;
}


void PenStyleWidget::setPen( const PenCmd::Pen &pen )
{
    m_pen = pen;
    slotReset();
}


void PenStyleWidget::apply()
{
    int flags = getPenConfigChange();

    if ( flags & PenCmd::LineEnd )
        m_pen.lineEnd = getLineEnd();

    if ( flags & PenCmd::LineBegin )
        m_pen.lineBegin = getLineBegin();

    if ( flags & PenCmd::Color )
        m_pen.pen.setColor( getPen().color() );

    if ( flags & PenCmd::Style )
        m_pen.pen.setStyle( getPen().style() );

    if ( flags & PenCmd::Width )
        m_pen.pen.setWidth( getPen().width() );
}


void PenStyleWidget::slotReset()
{
    setPen( m_pen.pen );
    m_ui->widthLabel->setEnabled( m_pen.pen.style() != NoPen );
    m_ui->widthInput->setEnabled( m_pen.pen.style() != NoPen );

    setLineBegin( m_pen.lineBegin );
    setLineEnd( m_pen.lineEnd );
}


void PenStyleWidget::slotPenChanged()
{
    QPen pen = getPen();
    m_ui->widthLabel->setEnabled( pen.style() != NoPen );
    m_ui->widthInput->setEnabled( pen.style() != NoPen );
    m_ui->pbPreview->setPen( pen );
}


void PenStyleWidget::slotLineBeginChanged()
{
    m_ui->pbPreview->setLineBegin( getLineBegin() );
}


void PenStyleWidget::slotLineEndChanged()
{
    m_ui->pbPreview->setLineEnd( getLineEnd() );
}

#include "penstylewidget.moc"
