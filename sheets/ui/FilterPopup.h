/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FILTER_POPUP
#define CALLIGRA_SHEETS_FILTER_POPUP

#include "sheets_ui_export.h"
#include <QFrame>

class QAbstractButton;

namespace Calligra
{
namespace Sheets
{
class CellBase;
class Database;
class Filter;

class CALLIGRA_SHEETS_UI_EXPORT FilterPopup : public QFrame
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    FilterPopup(QWidget *parent, const CellBase &cell, Database *database);

    static void showPopup(QWidget *parent, const CellBase &cell, const QRect &cellRect, Database *database);

    /**
     * Destructor.
     */
    ~FilterPopup() override;

    void updateFilter(Filter *filter) const;

protected:
    void closeEvent(QCloseEvent *) override;

private Q_SLOTS:
    void buttonClicked(QAbstractButton *button);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FILTER_POPUP
