/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPADisplayMasterShapesCommand.h"

#include <KLocalizedString>

#include "KoPAPage.h"

KoPADisplayMasterShapesCommand::KoPADisplayMasterShapesCommand(KoPAPage *page, bool display)
    : m_page(page)
    , m_display(display)
{
    if (m_display) {
        setText(kundo2_i18n("Show master shapes"));
    } else {
        setText(kundo2_i18n("Hide master shapes"));
    }
}

KoPADisplayMasterShapesCommand::~KoPADisplayMasterShapesCommand() = default;

void KoPADisplayMasterShapesCommand::redo()
{
    m_page->setDisplayMasterShapes(m_display);
    m_page->update();
}

void KoPADisplayMasterShapesCommand::undo()
{
    m_page->setDisplayMasterShapes(!m_display);
    m_page->update();
}
