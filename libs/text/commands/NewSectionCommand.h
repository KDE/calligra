// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014 Denis Kuplaykov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef NEWSECTIONCOMMAND_H
#define NEWSECTIONCOMMAND_H

#include <kundo2command.h>

class KoSection;
class QTextDocument;

// FIXME: why it is not going from KoTextCommandBase?
//  If it will be changed to KoTextCommandBase,
//  don't forget to add UndoRedoFinalizer.
class NewSectionCommand : public KUndo2Command
{
public:
    explicit NewSectionCommand(QTextDocument *document);
    ~NewSectionCommand() override;

    void undo() override;
    void redo() override;

private:
    bool m_first; ///< Checks first call of redo
    QTextDocument *m_document; ///< Pointer to document
    KoSection *m_section; ///< Inserted section
    int m_childIdx; ///< Position of inserted section in parent, after inserting
};

#endif // NEWSECTIONCOMMAND_H
