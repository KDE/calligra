/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef H_KW_SHAPE_CREATE_COMMAND_H
#define H_KW_SHAPE_CREATE_COMMAND_H

#include <kundo2command.h>
#include <words_export.h>

class KoShape;
class KWDocument;

/// The undo / redo command for creating shapes
class WORDS_EXPORT KWShapeCreateCommand : public KUndo2Command
{
public:
    /**
     * Command used on creation of new shapes
     * @param doc the document used to add/remove the shape from
     * @param shape the shape thats just been created.
     * @param parent the parent command used for macro commands
     */
    KWShapeCreateCommand(KWDocument *doc, KoShape *shape, KUndo2Command *parent = nullptr);
    ~KWShapeCreateCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KWDocument *m_document;
    KoShape *m_shape;
    bool m_deleteShape;
};

#endif // H_KW_SHAPE_CREATE_COMMAND_H
