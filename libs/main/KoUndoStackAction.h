/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOUNDOSTACKACTION_H
#define KOUNDOSTACKACTION_H

#include <QAction>

class KUndo2Stack;

class KoUndoStackAction : public QAction
{
    Q_OBJECT
public:
    enum Type {
        UNDO,
        RED0
    };

    KoUndoStackAction(KUndo2Stack* stack, Type type);

private Q_SLOTS:
    void slotUndoTextChanged(const QString& text);

private:
    Type m_type;
};
#endif // KOUNDOSTACKACTION_H
