/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SORT_DIALOG
#define CALLIGRA_SHEETS_SORT_DIALOG

#include "engine/Value.h"
#include <KoDialog.h>

class QTableWidgetItem;

namespace Calligra
{
namespace Sheets
{
class Localization;
class Selection;

/**
 * \ingroup UI
 * Dialog to set options for the sort cell values command.
 */
class SortDialog : public KoDialog
{
    Q_OBJECT
public:
    SortDialog(QWidget *parent, QRect rect, const QVector<Value> &firstRow, const QVector<Value> &firstCol);
    ~SortDialog() override;

    void setCustomLists(const QStringList &lsts, Localization *locale);

    bool sortRows() const;
    bool skipFirst() const;
    bool copyFormat() const;
    bool isHorizontal() const;

    bool useCustomList() const;
    QStringList customList() const;

    int criterionCount() const;
    int criterionIndex(int row) const;
    Qt::SortOrder criterionSortOrder(int row) const;
    Qt::CaseSensitivity criterionCaseSensitivity(int row) const;

public Q_SLOTS: // reimplementations
    void slotButtonClicked(int button) override;

private:
    void init();

private Q_SLOTS:
    void useHeaderChanged(bool);
    void orientationChanged(bool horizontal);
    void itemActivated(QTableWidgetItem *item);
    void itemSelectionChanged();
    void addCriterion();
    void removeCriterion();
    void moveCriterionUp();
    void moveCriterionDown();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SORT_DIALOG
