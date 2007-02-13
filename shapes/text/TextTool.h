/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOTEXTTOOL_H
#define KOTEXTTOOL_H

#include "TextShape.h"

#include <KoTextSelectionHandler.h>
#include <KoTool.h>

#include <QTextCursor>
#include <QAction>

class KoStyleManager;
class UndoTextCommand; // private class

/**
 * This is the tool for the text-shape (which is a flake-based plugin).
 */
class TextTool : public KoTool {
    Q_OBJECT
public:
    explicit TextTool(KoCanvasBase *canvas);
    ~TextTool();

    void paint( QPainter &painter, KoViewConverter &converter );

    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseDoubleClickEvent( KoPointerEvent *event );
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void activate (bool temporary=false);
    void deactivate();

    KoToolSelection* selection();
    QWidget *createOptionWidget();

signals:
    void styleManagerChanged(KoStyleManager *manager);
    void charFormatChanged(const QTextCharFormat &format);
    void blockFormatChanged(const QTextBlockFormat &format);

private slots:
    void nonbreakingSpace();
    void nonbreakingHyphen();
    void softHyphen();
    void lineBreak();
    void alignLeft();
    void alignRight();
    void alignCenter();
    void alignBlock();
    void superScript(bool);
    void subScript(bool);
    void decreaseIndent();
    void increaseIndent();

    void addUndoCommand();

private:
    void repaintCaret();
    void repaintSelection(int from, int to);
    QRectF textRect(int startPosition, int endPosition);
    int pointToPosition(const QPointF & point) const;
    void updateSelectionHandler();
    void updateActions();
    void updateStyleManager();
    void setShapeData(KoTextShapeData *data);

    void editingPluginEvents();
    void finishedWord();
    void finishedParagraph();

private:
    friend class UndoTextCommand;
    TextShape *m_textShape;
    KoTextShapeData *m_textShapeData;
    QTextCursor m_caret;
    KoTextSelectionHandler m_selectionHandler;
    bool m_allowActions;
    bool m_allowAddUndoCommand;
    int m_prevCursorPosition; /// used by editingPluginEvents

    QAction *m_actionFormatBold;
    QAction *m_actionFormatItalic;
    QAction *m_actionFormatUnderline;
    QAction *m_actionFormatStrikeOut;
    QAction *m_actionAlignLeft;
    QAction *m_actionAlignRight;
    QAction *m_actionAlignCenter;
    QAction *m_actionAlignBlock;
    QAction *m_actionFormatSuper;
    QAction *m_actionFormatSub;
    QAction *m_actionFormatIncreaseIndent;
    QAction *m_actionFormatDecreaseIndent;
};

#endif
