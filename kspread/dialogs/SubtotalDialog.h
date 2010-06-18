/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>

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


#ifndef KSPREAD_SUBTOTAL_DIALOG
#define KSPREAD_SUBTOTAL_DIALOG

#include <QRect>

#include <kdialog.h>

#include <ui_SubtotalWidget.h>


namespace KSpread
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to add subtotals.
 */
class SubtotalDialog : public KDialog, public ::Ui::SubtotalWidget
{
    Q_OBJECT

public:
    SubtotalDialog(QWidget* parent, Selection* selection);
    ~SubtotalDialog();

    QRect const & selection() const {
        return m_range;
    }
    Sheet * sheet() const {
        return m_pSheet;
    }

private slots:
    void slotOk();
    void slotCancel();
    void slotUser1();

private:
    Selection *      m_selection;
    Sheet *          m_pSheet;
    QRect            m_range;

    void fillColumnBoxes();
    void fillFunctionBox();
    void removeSubtotalLines();
    bool addSubtotal(int mainCol, int column, int row, int topRow,
                     bool addRow, QString const & text);
};

} // namespace KSpread

#endif // KSPREAD_SUBTOTAL_DIALOG
