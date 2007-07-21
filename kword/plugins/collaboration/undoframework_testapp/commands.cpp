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

#include <QtGui>

#include "commands.h"
#include "diagramitem.h"

MoveCommand::MoveCommand(DiagramItem *diagramItem, const QPointF &oldPos,
                 kcollaborate::UndoCommand *parent)
    : kcollaborate::UndoCommand(parent)
{
    myDiagramItem = diagramItem;
    newPos_ = diagramItem->pos();
    oldPos_ = oldPos;

    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos_)));
}

bool MoveCommand::mergeWith(const QUndoCommand* /*command*/)
{
    return false;
    /*
    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
    DiagramItem *item = moveCommand->myDiagramItem;

    if (myDiagramItem != item)
    return false;

    newPos = item->pos();

    return true;
    */
}

void MoveCommand::undo()
{
    myDiagramItem->setPos(oldPos_);
    myDiagramItem->scene()->update();
    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos_)));
}

void MoveCommand::redo()
{
    myDiagramItem->setPos(newPos_);
    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos_)));
}

QString MoveCommand::commandName() const
{
    return "move";
}

QString MoveCommand::data() const
{
    QString out;
    out.reserve(256);
    
    out.append("<oldPosition>").append(qpointfToString(oldPos_)).append("</oldPosition>");
    out.append("<newPosition>").append(qpointfToString(newPos_)).append("</newPosition>");
    
    return out;
}

DeleteCommand::DeleteCommand(QGraphicsScene *scene, kcollaborate::UndoCommand *parent)
    : kcollaborate::UndoCommand(parent)
{
    myGraphicsScene = scene;
    QList<QGraphicsItem *> list = myGraphicsScene->selectedItems();
    list.first()->setSelected(false);
    myDiagramItem = static_cast<DiagramItem *>(list.first());
    setText(QObject::tr("Delete %1")
        .arg(createCommandString(myDiagramItem, myDiagramItem->pos())));
}

void DeleteCommand::undo()
{
    myGraphicsScene->addItem(myDiagramItem);
    myGraphicsScene->update();
}

void DeleteCommand::redo()
{
    myGraphicsScene->removeItem(myDiagramItem);
}

QString DeleteCommand::commandName() const
{
    return "delete";
}

QString DeleteCommand::data() const
{
    return "";
}

AddCommand::AddCommand(DiagramItem::DiagramType addType,
                       QGraphicsScene *scene, kcollaborate::UndoCommand *parent)
    : kcollaborate::UndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;
    myDiagramItem = new DiagramItem(addType);
    initialPosition = QPointF((itemCount * 15) % int(scene->width()),
                              (itemCount * 15) % int(scene->height()));
    scene->update();
    ++itemCount;
    const QColor& color= myDiagramItem->color();
    setText(QObject::tr("Add %1 with color (%2,%3,%4,%5)")
        .arg(createCommandString(myDiagramItem, initialPosition))
	.arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
}

void AddCommand::undo()
{
    myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
}

void AddCommand::redo()
{
    myGraphicsScene->addItem(myDiagramItem);
    myDiagramItem->setPos(initialPosition);
    myGraphicsScene->clearSelection();
    myGraphicsScene->update();
}

QString AddCommand::commandName() const
{
    return "create";
}

QString AddCommand::data() const
{
    QString out;
    out.reserve(256);
    
    out.append(qcolorToString(myDiagramItem->color()));
    out.append("<newPosition>").append(qpointfToString(initialPosition)).append("</newPosition>");
    
    return out;
}

QString createCommandString(DiagramItem *item, const QPointF &pos)
{
    return QObject::tr("%1 at (%2, %3)")
        .arg(item->diagramType() == DiagramItem::Box ? "Box" : "Triangle")
        .arg(pos.x()).arg(pos.y());
}

QString qpointfToString(const QPointF &p)
{
    QString out("<point x=\"%1\" y=\"%2\" />");
    return out.arg(p.x()).arg(p.y());
}

QString qcolorToString(const QColor &c)
{
    QString out("<color red=\"%1\" green=\"%2\" blue=\"%3\" alpha=\"%4\" />");
    return out.arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}
