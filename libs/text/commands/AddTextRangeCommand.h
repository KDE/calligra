// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef ADDTEXTRANGECOMMAND_H
#define ADDTEXTRANGECOMMAND_H

#include <kundo2command.h>

class KoTextRange;

class AddTextRangeCommand : public KUndo2Command
{
public:
    explicit AddTextRangeCommand(KoTextRange *range, KUndo2Command *parent = nullptr);
    ~AddTextRangeCommand() override;

    void undo() override;
    void redo() override;

private:
    KoTextRange *m_range;
};

#endif // ADDTEXTRANGECOMMAND_H
