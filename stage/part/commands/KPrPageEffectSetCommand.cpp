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

#include "KPrPageEffectSetCommand.h"

#include <klocalizedstring.h>

#include "KPrPage.h"
#include "KPrPageApplicationData.h"
#include "pageeffects/KPrPageEffect.h"

KPrPageEffectSetCommand::KPrPageEffectSetCommand(KoPAPageBase * page, KPrPageEffect * pageEffect, KUndo2Command *parent)
: KUndo2Command(parent)
, m_page( page )
, m_newPageEffect( pageEffect )
, m_oldPageEffect( KPrPage::pageData( m_page )->pageEffect() )
, m_deleteNewPageEffect( true )
{
    Q_ASSERT( m_newPageEffect != m_oldPageEffect );
    if ( m_newPageEffect ) {
        if ( ! m_oldPageEffect ) {
            setText( kundo2_i18n( "Create Slide Effect" ) );
        }
        else {
            setText( kundo2_i18n( "Modify Slide Effect" ) );
        }
    }
    else {
        setText( kundo2_i18n( "Delete Slide Effect" ) );
    }
}

KPrPageEffectSetCommand::~KPrPageEffectSetCommand()
{
    if ( m_deleteNewPageEffect ) {
        delete m_newPageEffect;
    }
    else {
        delete m_oldPageEffect;
    }
}

void KPrPageEffectSetCommand::redo()
{
    KPrPage::pageData( m_page )->setPageEffect( m_newPageEffect );
    m_deleteNewPageEffect = false;
}

void KPrPageEffectSetCommand::undo()
{
    KPrPage::pageData( m_page )->setPageEffect( m_oldPageEffect );
    m_deleteNewPageEffect = true;
}
