// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "RenameSectionCommand.h"
#include <KoSection.h>
#include <KoSectionModel.h>
#include <KoTextDocument.h>

#include <KLocalizedString>
#include <kundo2command.h>

RenameSectionCommand::RenameSectionCommand(KoSection *section, const QString &newName, QTextDocument *document)
    : KUndo2Command()
    , m_sectionModel(KoTextDocument(document).sectionModel())
    , m_section(section)
    , m_newName(newName)
    , m_first(true)
{
    setText(kundo2_i18n("Rename Section"));
}

RenameSectionCommand::~RenameSectionCommand() = default;

void RenameSectionCommand::undo()
{
    KUndo2Command::undo();
    m_sectionModel->setName(m_section, m_oldName);
}

void RenameSectionCommand::redo()
{
    if (!m_first) {
        KUndo2Command::redo();
    }
    m_oldName = m_section->name();
    m_sectionModel->setName(m_section, m_newName);
    m_first = false;
}

int RenameSectionCommand::id() const
{
    // FIXME: extract this to some enum shared across all commands
    return 34537684;
}

bool RenameSectionCommand::mergeWith(const KUndo2Command *other)
{
    if (other->id() != id()) {
        return false;
    }

    const RenameSectionCommand *command = static_cast<const RenameSectionCommand *>(other);
    if (command->m_section != m_section || m_newName != command->m_oldName) {
        return false;
    }
    m_newName = command->m_oldName;
    return true;
}
