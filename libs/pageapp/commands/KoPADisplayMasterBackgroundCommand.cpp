/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPADisplayMasterBackgroundCommand.h"

#include <KLocalizedString>

#include "KoPAPage.h"

KoPADisplayMasterBackgroundCommand::KoPADisplayMasterBackgroundCommand(KoPAPage *page, bool display)
    : m_page(page)
    , m_display(display)
{
    if (m_display) {
        setText(kundo2_i18n("Display master background"));
    } else {
        if (m_page->pageType() == KoPageApp::Slide) {
            setText(kundo2_i18n("Display slide background"));
        } else {
            setText(kundo2_i18n("Display page background"));
        }
    }
}

KoPADisplayMasterBackgroundCommand::~KoPADisplayMasterBackgroundCommand() = default;

void KoPADisplayMasterBackgroundCommand::redo()
{
    m_page->setDisplayMasterBackground(m_display);
    m_page->update();
}

void KoPADisplayMasterBackgroundCommand::undo()
{
    m_page->setDisplayMasterBackground(!m_display);
    m_page->update();
}
