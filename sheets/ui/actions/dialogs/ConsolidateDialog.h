/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000-2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CONSOLIDATE_DIALOG
#define CALLIGRA_SHEETS_CONSOLIDATE_DIALOG

#include <KoDialog.h>

#include "engine/Region.h"

namespace Calligra
{
namespace Sheets
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to consolidate cell values.
 *
 * How it works:
 * \li source cell ranges have to be of the same size
 * \li the function works with the relative (unless headers are defined) cell
 * references in the source cell ranges
 * \li the first columns/rows can be handled as headers: then, the function is
 * only applied to values, that have the same headings
 * \li optionally you can link to the source data: then, formulas are used in
 * the target cell range. Otherwise, only the formula results get stored.
 */
class ConsolidateDialog : public KoDialog
{
    Q_OBJECT
public:
    ConsolidateDialog(QWidget *parent, Selection *selection);
    ~ConsolidateDialog() override;

    QList<Region> sourceRegions() const;
    Region targetRegion() const;
    QString function() const;
    bool rowHeaders() const;
    bool colHeaders() const;

Q_SIGNALS:
    void applyChange();

public Q_SLOTS:
    void accept() override;

private Q_SLOTS:
    void slotAdd();
    void slotRemove();

    void slotSelectionChanged();
    void slotReturnPressed();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CONSOLIDATE_DIALOG
