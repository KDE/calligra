// This file is part of the KDE project
// SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef DELETEANCHORSCOMMAND_H
#define DELETEANCHORSCOMMAND_H

#include <kundo2command.h>

#include <QList>

class QTextDocument;
class KoShapeAnchor;
class KoAnchorInlineObject;
class KoAnchorTextRange;

class DeleteAnchorsCommand : public KUndo2Command
{
public:
    DeleteAnchorsCommand(const QList<KoShapeAnchor *> &anchors, QTextDocument *document, KUndo2Command *parent);
    ~DeleteAnchorsCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<KoAnchorInlineObject *> m_anchorObjects;
    QList<KoAnchorTextRange *> m_anchorRanges;
    QTextDocument *m_document;
    bool m_first;
    bool m_deleteAnchors;
};

#endif /* DELETEANCHORSCOMMAND_H */
