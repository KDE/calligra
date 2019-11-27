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


#ifndef CALLIGRA_SHEETS_REGION_SELECTOR
#define CALLIGRA_SHEETS_REGION_SELECTOR

#include <QWidget>

#include "sheets_common_export.h"

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
class CALLIGRA_SHEETS_COMMON_EXPORT RegionSelector : public QWidget
{
    Q_OBJECT

public:
    enum SelectionMode { SingleCell = 0, MultipleCells = 1 }; // TODO Stefan: merge with Selection::Mode
    enum DisplayMode { Widget, Dialog };

    explicit RegionSelector(QWidget *parent = 0);
    ~RegionSelector() override;

    void setSelectionMode(SelectionMode mode);
    void setSelection(Selection* selection);
    void setDialog(QDialog* dialog);
    void setLabel(const QString& text);

    KTextEdit* textEdit() const;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

protected Q_SLOTS:
    void switchDisplayMode(bool state);
    void choiceChanged();

private:
    Q_DISABLE_COPY(RegionSelector)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_REGION_SELECTOR
