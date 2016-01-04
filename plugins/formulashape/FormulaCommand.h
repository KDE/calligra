/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef FORMULACOMMAND_H
#define FORMULACOMMAND_H

#include <kundo2command.h>
#include <QList>
#include <QHash>
#include <QMetaType>

class KoFormulaShape;

/**
 *
 * All FormulaCommands are used to manipulate the formula in various ways.
 * They all provide a redo and undo method as well as changeCursor method
 * which sets the cursor after the current action. A extra method for this is necessary,
 * as there might be no cursor when a undo/redo is done, because the tool was deactivated
 *
 **/

class FormulaCommand :  public KUndo2Command {
public:
    explicit FormulaCommand(KoFormulaShape* formulaShape,
                            QString mathML, QFont defaultFont,
                            QColor foregroundColor, QColor backgroundColor,
                            KUndo2Command *parent=0);
    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();
private:
    KoFormulaShape* m_shape;
    QString m_oldMathML;
    QString m_newMathML;
    QFont m_oldDefaultFont;
    QFont m_newDefaultFont;
    QColor m_oldForegroundColor;
    QColor m_newForegroundColor;
    QColor m_oldBackgroundColor;
    QColor m_newBackgroundColor;
};

#endif // FORMULACOMMAND_H
