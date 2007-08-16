/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StarShapeConfigWidget.h"
#include "KoStarShape.h"

StarShapeConfigWidget::StarShapeConfigWidget()
{
    widget.setupUi( this );
    layout()->setContentsMargins( 0,0,0,0 );
}

void StarShapeConfigWidget::setUnit(KoUnit unit)
{
    widget.innerRadius->setUnit( unit );
    widget.outerRadius->setUnit( unit );
}

void StarShapeConfigWidget::open(KoShape *shape)
{
    m_star = dynamic_cast<KoStarShape*>( shape );
    if( ! m_star )
        return;

    widget.corners->setValue( m_star->cornerCount() );
    widget.innerRadius->setValue( m_star->baseRadius() );
    widget.outerRadius->setValue( m_star->tipRadius() );
}

void StarShapeConfigWidget::save()
{
    if( ! m_star )
        return;

    m_star->setCornerCount( widget.corners->value() );
    m_star->setBaseRadius( widget.innerRadius->value() );
    m_star->setTipRadius( widget.outerRadius->value() );
}

#include "StarShapeConfigWidget.moc"
