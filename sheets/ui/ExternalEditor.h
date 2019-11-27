/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>

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


#ifndef CALLIGRA_SHEETS_EXTERNAL_EDITOR
#define CALLIGRA_SHEETS_EXTERNAL_EDITOR

#include <ktextedit.h>

#include "sheets_common_export.h"

class QFocusEvent;
class QKeyEvent;
class QWidget;

namespace Calligra
{
namespace Sheets
{
class CellToolBase;

class ExternalEditor : public KTextEdit
{
    Q_OBJECT
public:
    explicit ExternalEditor(QWidget *parent = 0);
    ~ExternalEditor() override;

    QSize sizeHint() const override;

    void setCellTool(CellToolBase* cellTool);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    QAction* applyAction() const;
    QAction* cancelAction() const;
Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void applyChanges();
    void discardChanges();
    void setText(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private Q_SLOTS:
    void slotTextChanged();
    void slotCursorPositionChanged();

private:
    Q_DISABLE_COPY(ExternalEditor)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_EXTERNAL_EDITOR
