/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_REGION_SELECTOR
#define CALLIGRA_SHEETS_REGION_SELECTOR

#include <QWidget>

#include "sheets_ui_export.h"

class KTextEdit;

class QDialog;
class QEvent;
class QObject;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * A minimizable line edit for choosing cell regions.
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class CALLIGRA_SHEETS_UI_EXPORT RegionSelector : public QWidget
{
    Q_OBJECT

public:
    enum SelectionMode { SingleCell = 0, MultipleCells = 1 }; // TODO Stefan: merge with Selection::Mode
    enum DisplayMode { Widget, Dialog };

    explicit RegionSelector(QWidget *parent = nullptr);
    ~RegionSelector() override;

    void setSelectionMode(SelectionMode mode);
    void setSelection(Selection *selection);
    void setDialog(QDialog *dialog);
    void setLabel(const QString &text);

    KTextEdit *textEdit() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

protected Q_SLOTS:
    void switchDisplayMode(bool state);
    void choiceChanged();

private:
    Q_DISABLE_COPY(RegionSelector)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_REGION_SELECTOR
