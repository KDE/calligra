/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_DIALOG
#define CALLIGRA_SHEETS_ACTION_DIALOG

#include <KoDialog.h>

class QWidget;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * Base class for non-modal action dialogs.
 */
class ActionDialog : public KoDialog
{
    Q_OBJECT

public:
    // constructor
    ActionDialog(QWidget *parent, ButtonCodes extraButtons = None);

    ~ActionDialog() override;

    /** Called if selection changes. */
    virtual void onSelectionChanged(Selection *)
    {
    }

protected:
    virtual void onApply() = 0;
    virtual void onClose()
    {
    }
    /** Widget with the default focus. */
    virtual QWidget *defaultWidget()
    {
        return nullptr;
    }

    virtual void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void slotClose();
    void slotApply();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG
