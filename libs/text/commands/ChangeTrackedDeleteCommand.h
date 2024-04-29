/*
 This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later/

#ifndef CHANGETRACKEDDELETECOMMAND_H
#define CHANGETRACKEDDELETECOMMAND_H

#include "KoTextCommandBase.h"
#include <KoListStyle.h>
#include <QList>
#include <QPointer>

class QTextDocument;
class QTextCursor;
class KoShapeController;
class KoDocumentRdfBase;
class KoTextEditor;

class ChangeTrackedDeleteCommand : public KoTextCommandBase
{
public:
    enum DeleteMode {
        PreviousChar,
        NextChar
    };

    ChangeTrackedDeleteCommand(DeleteMode mode,
                               QTextDocument *document,
                               KoShapeController *shapeController,
                               KUndo2Command* parent = nullptr);
    virtual ~ChangeTrackedDeleteCommand();

    virtual void undo();
    virtual void redo();

    virtual int id() const;
    virtual bool mergeWith ( const KUndo2Command *command);

private:
    QPointer<QTextDocument> m_document;
    KoDocumentRdfBase *m_rdf;
    KoShapeController *m_shapeController;
    bool m_first;
    bool m_undone;
    bool m_canMerge;
    DeleteMode m_mode;
    QList<int> m_removedElements;
    QList<KoListStyle::ListIdType> m_newListIds;
    int m_position, m_length;
    int m_addedChangeElement;

    virtual void deleteChar();
    virtual void deletePreviousChar();
    virtual void deleteSelection(KoTextEditor *editor);
    virtual void removeChangeElement(int changeId);
    virtual void updateListIds(QTextCursor &cursor);
    virtual void updateListChanges();
    virtual void handleListItemDelete(KoTextEditor *editor);
};

#endif // CHANGETRACKEDDELETECOMMAND_H
