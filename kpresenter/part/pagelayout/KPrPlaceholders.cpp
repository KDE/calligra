/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPlaceholders.h"

KPrPlaceholders::KPrPlaceholders()
:m_layout( 0 )
{
}

KPrPlaceholders::~KPrPlaceholders()
{
}

void KPrPlaceholders::setLayout( KPrPageLayout * layout, KoPADocument * document, const QList<KoShape *> & shapes )
{
    m_layout = layout;
// check if we are initialized if not walk the shapes and get the data
// check if we have enough presentation of the classes we need
// remove all other placeholders
// this needs to create commands to do all the work
// we need to make sure undo/redo is working
}

void KPrPlaceholders::shapeAdded( KoShape * shape )
{
// if presentation:class add to index no matter if it is a placeholder or not
}

void KPrPlaceholders::shapeRemoved( KoShape * shape )
{
// if it is a placeholder remove it
// if presentation:class is set and not a placeholder remove it and add a placeholder
//    this needs to be checked as on undo/redo we might get a problem
// other do nothing
}

void KPrPlaceholders::initialize( const QList<KoShape *> & shapes )
{
    if ( m_initialized ) {
        return;
    }
}
