/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_SORT_DIALOG
#define KSPREAD_SORT_DIALOG

#include <KDialog>

class QTableWidgetItem;

namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog to set options for the sort cell values command.
 */
class SortDialog : public KDialog
{
    Q_OBJECT
public:
    SortDialog(QWidget* parent, Selection* selection);
    ~SortDialog();

public Q_SLOTS: // reimplementations
    virtual void accept();
    virtual void slotButtonClicked(int button);

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

} // namespace KSpread

#endif // KSPREAD_SORT_DIALOG
