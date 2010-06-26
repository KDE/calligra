/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <koffice-devel@kde.org>

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


#ifndef KSPREAD_EXTERNAL_EDITOR
#define KSPREAD_EXTERNAL_EDITOR

#include <KTextEdit>

#include "kspread_export.h"

class QFocusEvent;
class QKeyEvent;
class QWidget;

namespace KSpread
{
class CellToolBase;

class ExternalEditor : public KTextEdit
{
    Q_OBJECT
public:
    ExternalEditor(QWidget* parent = 0);
    ~ExternalEditor();

    virtual QSize sizeHint() const;

    void setCellTool(CellToolBase* cellTool);

Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void applyChanges();
    void discardChanges();
    void setText(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private slots:
    void slotTextChanged();
    void slotCursorPositionChanged();

private:
    Q_DISABLE_COPY(ExternalEditor)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_EXTERNAL_EDITOR
