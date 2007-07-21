/****************************************************************************
**
** Copyright (C) 2007-2007 Trolltech ASA. All rights reserved.
** Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
**
** This file was part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef COMMANDS_H
#define COMMANDS_H

#include <libcollaboration/UndoCommand.h>

#include "diagramitem.h"

class MoveCommand : public kcollaborate::UndoCommand
{
public:
    enum { Id = 1234 };

    MoveCommand(DiagramItem *diagramItem, const QPointF &oldPos,
                kcollaborate::UndoCommand *parent = 0);

    void undo();
    void redo();
    bool mergeWith(const QUndoCommand *command);
    int id() const { return Id; }

    virtual QString commandName() const;
    virtual QString data() const;

private:
    DiagramItem *myDiagramItem;
    QPointF oldPos_;
    QPointF newPos_;
};

class DeleteCommand : public kcollaborate::UndoCommand
{
public:
    DeleteCommand(QGraphicsScene *graphicsScene, kcollaborate::UndoCommand *parent = 0);

    void undo();
    void redo();

    virtual QString commandName() const;
    virtual QString data() const;

private:
    DiagramItem *myDiagramItem;
    QGraphicsScene *myGraphicsScene;
};

class AddCommand : public kcollaborate::UndoCommand
{
public:
    AddCommand(DiagramItem::DiagramType addType, QGraphicsScene *graphicsScene,
               kcollaborate::UndoCommand *parent = 0);

    void undo();
    void redo();

    virtual QString commandName() const;
    virtual QString data() const;

private:
    DiagramItem *myDiagramItem;
    QGraphicsScene *myGraphicsScene;
    QPointF initialPosition;
};

QString createCommandString(DiagramItem *item, const QPointF &point);

QString qpointfToString(const QPointF &point);
QString qcolorToString(const QColor &color);
#endif
