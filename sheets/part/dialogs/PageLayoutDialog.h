/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PAGE_LAYOUT_DIALOG
#define CALLIGRA_SHEETS_PAGE_LAYOUT_DIALOG

#include <KoPageLayoutDialog.h>

namespace Calligra
{
namespace Sheets
{
class Sheet;

/**
 * \ingroup UI
 * Dialog to set the page layout.
 */
class PageLayoutDialog : public KoPageLayoutDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    explicit PageLayoutDialog(QWidget *parent, Sheet *sheet);

    /**
     * Destructor.
     */
    ~PageLayoutDialog() override;

public Q_SLOTS:
    void accept() override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PAGE_LAYOUT_DIALOG
