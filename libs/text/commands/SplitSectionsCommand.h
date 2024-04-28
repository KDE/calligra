// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Denis Kuplaykov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef SPLITSECTIONSCOMMAND_H
#define SPLITSECTIONSCOMMAND_H

#include <kundo2command.h>

class KoSection;
class QTextDocument;

// FIXME: why it is not going from KoTextCommandBase?
//  If it will be changed to KoTextCommandBase,
//  don't forget to add UndoRedoFinalizer.
class SplitSectionsCommand : public KUndo2Command
{
public:
    enum SplitType { Startings, Endings };

    explicit SplitSectionsCommand(QTextDocument *document, SplitType type, int splitPosition);
    ~SplitSectionsCommand() override;

    void undo() override;
    void redo() override;

private:
    bool m_first; ///< Checks first call of redo
    QTextDocument *m_document; ///< Pointer to document
    SplitType m_type; ///< Split type
    int m_splitPosition; ///< Split position
};

#endif // SPLITSECTIONSCOMMAND_H
