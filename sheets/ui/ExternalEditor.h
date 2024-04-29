/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_EXTERNAL_EDITOR
#define CALLIGRA_SHEETS_EXTERNAL_EDITOR

#include "sheets_ui_export.h"
#include <ktextedit.h>

class QFocusEvent;
class QKeyEvent;
class QWidget;

namespace Calligra
{
namespace Sheets
{
class CellToolBase;

class CALLIGRA_SHEETS_UI_EXPORT ExternalEditor : public KTextEdit
{
    Q_OBJECT
public:
    explicit ExternalEditor(QWidget *parent = nullptr);
    ~ExternalEditor() override;

    QSize sizeHint() const override;

    void setCellTool(CellToolBase *cellTool);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    QAction *applyAction() const;
    QAction *cancelAction() const;
Q_SIGNALS:
    void textModified(const QString &text);

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
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_EXTERNAL_EDITOR
