// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later/

#ifndef SHOWCHANGECOMMAND_H
#define SHOWCHANGECOMMAND_H

#include "KoTextCommandBase.h"
#include <QList>
#include <QObject>

class KoChangeTracker;
class KoTextEditor;
class KoCanvasBase;

class QTextDocument;

class ShowChangesCommand : public QObject, public KoTextCommandBase
{
    Q_OBJECT
public:
    ShowChangesCommand(bool showChanges, QTextDocument *document, KoCanvasBase *canvas, KUndo2Command *parent = nullptr);
    ~ShowChangesCommand() override;

    void undo() override;
    void redo() override;

Q_SIGNALS:
    void toggledShowChange(bool on);

private:
    void enableDisableChanges();
    void enableDisableStates(bool showChanges);
    void insertDeletedChanges();
    void checkAndAddAnchoredShapes(int position, int length);
    void removeDeletedChanges();
    void checkAndRemoveAnchoredShapes(int position, int length);

    QTextDocument *m_document;
    KoChangeTracker *m_changeTracker;
    KoTextEditor *m_textEditor;
    bool m_first;
    bool m_showChanges;
    KoCanvasBase *m_canvas;

    QList<KUndo2Command *> m_shapeCommands;
};

#endif // SHOWCHANGECOMMAND_H
