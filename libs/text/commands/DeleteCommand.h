// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef DELETECOMMAND_H
#define DELETECOMMAND_H

#include "KoTextCommandBase.h"

#include <QHash>
#include <QPointer>
#include <QSet>
#include <QTextCharFormat>

class QTextDocument;

class KoShapeController;
class KoInlineObject;
class KoTextRange;
class KoSection;

class DeleteVisitor;

class DeleteCommand : public KoTextCommandBase
{
public:
    enum DeleteMode { PreviousChar, NextChar };

    DeleteCommand(DeleteMode mode, QTextDocument *document, KoShapeController *shapeController, KUndo2Command *parent = nullptr);
    ~DeleteCommand() override;

    void undo() override;
    void redo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    friend class DeleteVisitor;

    struct SectionDeleteInfo {
        SectionDeleteInfo(KoSection *_section, int _childIdx)
            : section(_section)
            , childIdx(_childIdx)
        {
        }

        bool operator<(const SectionDeleteInfo &other) const;

        KoSection *section; ///< Section to remove
        int childIdx; ///< Position of section in parent's children() list
    };

    QPointer<QTextDocument> m_document;
    KoShapeController *m_shapeController;

    QSet<KoInlineObject *> m_invalidInlineObjects;
    QList<QTextCursor> m_cursorsToWholeDeleteBlocks;
    QMultiHash<int, KoTextRange *> m_rangesToRemove;
    QList<SectionDeleteInfo> m_sectionsToRemove;

    bool m_first;
    DeleteMode m_mode;
    int m_position;
    int m_length;
    QTextCharFormat m_format;
    bool m_mergePossible;

    void doDelete();
    void deleteInlineObject(KoInlineObject *object);
    bool checkMerge(const KUndo2Command *command);
    void updateListChanges();
    void finalizeSectionHandling(QTextCursor *caret, DeleteVisitor &visitor);
    void deleteSectionsFromModel();
    void insertSectionsToModel();
};

#endif // DELETECOMMAND_H
