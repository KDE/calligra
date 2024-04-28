/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG
#define CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG

#include "ActionDialog.h"

#include "ui_SpecialPasteWidget.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog for the special paste action.
 */
class SpecialPasteDialog : public ActionDialog, public Ui::SpecialPasteWidget
{
    Q_OBJECT
public:
    explicit SpecialPasteDialog(QWidget *parent);

    bool wantEverything() const;
    bool wantText() const;
    bool wantFormat() const;
    bool wantNoBorder() const;
    bool wantComment() const;
    bool wantResult() const;
    bool opOverwrite() const;
    bool opAdd() const;
    bool opSub() const;
    bool opMul() const;
    bool opDiv() const;

Q_SIGNALS:
    void paste();

public Q_SLOTS:
    void slotToggled(bool);

protected:
    virtual void onApply() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SPECIAL_PASTE_DIALOG
