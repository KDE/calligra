// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "NewSectionCommand.h"
#include <KoParagraphStyle.h>
#include <KoSection.h>
#include <KoSectionEnd.h>
#include <KoSectionModel.h>
#include <KoSectionUtils.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <KLocalizedString>
#include <kundo2command.h>

NewSectionCommand::NewSectionCommand(QTextDocument *document)
    : KUndo2Command()
    , m_first(true)
    , m_document(document)
{
    setText(kundo2_i18n("New Section"));
}

NewSectionCommand::~NewSectionCommand() = default;

void NewSectionCommand::undo()
{
    KUndo2Command::undo();
    // FIXME: if it will go to KoTextCommandBase, place UndoRedoFinalizer here

    // All formatting changes will be undone automatically.
    // Lets handle Model Level (see KoSectionModel).
    KoTextDocument(m_document).sectionModel()->deleteFromModel(m_section);
}

void NewSectionCommand::redo()
{
    KoTextDocument koDocument(m_document);
    KoSectionModel *sectionModel = koDocument.sectionModel();

    if (!m_first) {
        KUndo2Command::redo();
        // FIXME: if it will go to KoTextCommandBase, place UndoRedoFinalizer here

        // All formatting changes will be redone automatically.
        // Lets handle Model Level (see KoSectionModel).
        sectionModel->insertToModel(m_section, m_childIdx);
    } else {
        m_first = false;

        KoTextEditor *editor = koDocument.textEditor();
        editor->newLine();

        m_section = sectionModel->createSection(editor->constCursor(), sectionModel->sectionAtPosition(editor->constCursor().position()));
        m_childIdx = sectionModel->findRowOfChild(m_section);

        KoSectionEnd *sectionEnd = sectionModel->createSectionEnd(m_section);
        QTextBlockFormat fmt = editor->blockFormat();

        QList<KoSection *> sectionStartings = KoSectionUtils::sectionStartings(fmt);
        QList<KoSectionEnd *> sectionEndings = KoSectionUtils::sectionEndings(fmt);

        sectionStartings.append(m_section);
        sectionEndings.prepend(sectionEnd);

        KoSectionUtils::setSectionStartings(fmt, sectionStartings);
        KoSectionUtils::setSectionEndings(fmt, sectionEndings);

        editor->setBlockFormat(fmt);
    }
}
