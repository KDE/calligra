/* This file is part of the KDE project
 * Copyright ( C ) 2020 Dag Andersen <danders@get2net.dk>
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

#include "KPrPageTransitionSetCommand.h"

#include <klocalizedstring.h>

#include "KPrPage.h"

KPrPageTransitionSetCommand::KPrPageTransitionSetCommand(KoPAPageBase *page, const KPrPageTransition &transition, KUndo2Command *parent)
: KUndo2Command(parent)
, m_page( page )
, m_new(transition)
{
    m_old = KPrPage::pageData( m_page )->pageTransition();
    setText(kundo2_i18n("Modify Slide Transition"));
}

void KPrPageTransitionSetCommand::redo()
{
    KPrPage::pageData( m_page )->setPageTransition(m_new);
}

void KPrPageTransitionSetCommand::undo()
{
    KPrPage::pageData( m_page )->setPageTransition(m_old);
}
