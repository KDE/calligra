/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrAnimationCreateCommand.h"

#include <klocale.h>

#include "KPrDocument.h"
#include "shapeanimations/KPrShapeAnimationOld.h"
#include "KPrShapeAnimations.h"
#include "animations/KPrShapeAnimation.h"

KPrAnimationCreateCommand::KPrAnimationCreateCommand( KPrDocument * doc, KPrShapeAnimation * animation )
: m_doc( doc )
, m_animation( animation )
, m_deleteAnimation( true )
{
    setText( i18n( "Create shape animation" ) );
}

KPrAnimationCreateCommand::~KPrAnimationCreateCommand()
{
    if ( m_deleteAnimation ) {
        delete m_animation;
    }
}

void KPrAnimationCreateCommand::redo ()
{
    m_doc->addAnimation( m_animation );
    m_deleteAnimation = false;
}

void KPrAnimationCreateCommand::undo ()
{
    m_doc->removeAnimation( m_animation );
    m_deleteAnimation = true;
}
