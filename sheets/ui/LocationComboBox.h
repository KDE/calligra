/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LOCATION_COMBOBOX
#define CALLIGRA_SHEETS_LOCATION_COMBOBOX

#include "sheets_ui_export.h"
#include <KComboBox>

class QKeyEvent;

namespace Calligra
{
namespace Sheets
{
class Selection;
class Sheet;

/**
 * \class LocationComboBox
 * \ingroup UI
 * The combobox, that shows the address string of the current cell selection.
 *
 * Depending on the sheet settings the address is displayed in normal form
 * (e.g. A1 or B1:C3) or in LC (Line/Column) form (e.g. L1xC1 or 3Lx2C).
 */
class CALLIGRA_SHEETS_UI_EXPORT LocationComboBox : public KComboBox
{
    Q_OBJECT
public:
    explicit LocationComboBox(QWidget *parent = nullptr);

    void setSelection(Selection *selection);

    void addCompletionItem(const QString &_item);
    void removeCompletionItem(const QString &_item);

    /**
     * Updates the address string according to the current cell selection
     * and the current address mode (normal or LC mode).
     */
    void updateAddress();

public Q_SLOTS:
    void slotAddAreaName(const QString &);
    void slotRemoveAreaName(const QString &);

protected: // reimplementations
    void keyPressEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void updateAccessedCellRange(Sheet *sheet, const QPoint &location);

private Q_SLOTS:
    void slotActivateItem();
    void slotSelectionChanged();
    void slotActiveSheetChanged(Sheet *sheet);

private:
    bool activateItem();

private:
    QPointer<Selection> m_selection;
    KCompletion completionList;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LOCATION_COMBOBOX
