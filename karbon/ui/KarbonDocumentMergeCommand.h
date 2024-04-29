/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONDOCUMENTMERGECOMMAND_H_
#define _KARBONDOCUMENTMERGECOMMAND_H_

#include <kundo2command.h>

class KarbonDocument;

class KarbonDocumentMergeCommand : public KUndo2Command
{
public:
    KarbonDocumentMergeCommand(KarbonDocument *targetPart, KarbonDocument &sourcePart, KUndo2Command *parent = nullptr);
    ~KarbonDocumentMergeCommand() override = default;
    void redo() override;
    void undo() override;
};

#endif // _KARBONDOCUMENTMERGECOMMAND_H_
