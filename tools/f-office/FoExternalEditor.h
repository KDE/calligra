/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef FOEXTERNAL_EDITOR
#define FOEXTERNAL_EDITOR
#include <QTextEdit>

#include "ui/CellToolBase.h"

namespace KSpread
{
    class CellToolBase;
    class CellEditorBase;
}
using KSpread::CellEditorBase;
using KSpread::CellToolBase;

class FoExternalEditor : public QTextEdit
{
    Q_OBJECT
public:
    FoExternalEditor(QWidget* parent = 0);
    ~FoExternalEditor();

    void setCellTool(KSpread::CellToolBase* cellTool);
    void insertOperator(QString opreatorCharacter);

Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void applyChanges();
    void discardChanges();
    void setText(const QString &text);
    void setCursorPosition(int position);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private slots:
    void slotTextChanged();
    void slotCursorPositionChanged();

private:
    KSpread::CellToolBase* cellTool;
    bool isArray;
};

#endif
