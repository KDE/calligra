/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ShowColRowDialog.h"

// Qt
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>

// KF5
#include <KLocalizedString>

// Sheets
#include "engine/calligra_sheets_limits.h"
#include "engine/CellBase.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "../ui/Selection.h"

// commands
#include "../commands/RowColumnManipulators.h"

using namespace Calligra::Sheets;

ShowColRow::ShowColRow(QWidget* parent, Selection* selection, Type _type)
        : KoDialog(parent)
{
    setModal(true);
    setButtons(Ok | Cancel);
    m_selection = selection;
    typeShow = _type;

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QLabel *label = new QLabel(page);

    if (_type == Column) {
        setWindowTitle(i18n("Show Columns"));
        label->setText(i18n("Select hidden columns to show:"));
    } else if (_type == Row) {
        setWindowTitle(i18n("Show Rows"));
        label->setText(i18n("Select hidden rows to show:"));
    }

    list = new QListWidget(page);

    lay1->addWidget(label);
    lay1->addWidget(list);

    bool showColNumber = m_selection->activeSheet()->getShowColumnNumber();
    if (_type == Column) {
        QString text;
        QStringList listCol;
        int lastCol, col = 1;
        while (col <= KS_colMax) {
            if (m_selection->activeSheet()->columnFormats()->isHidden(col, &lastCol)) {
                for (int i = col; i <= lastCol; ++i)
                    listInt.append(i);
            }
            col = lastCol+1;
        }

        for (QList<int>::ConstIterator it = listInt.constBegin(); it != listInt.constEnd(); ++it) {
            if (!showColNumber)
                listCol += i18n("Column: %1", CellBase::columnName(*it));
            else
                listCol += i18n("Column: %1", text.setNum(*it));
        }
        list->addItems(listCol);
    } else if (_type == Row) {
        QString text;
        QStringList listRow;
        int lastRow, row = 1;
        while (row <= KS_rowMax) {
            if (m_selection->activeSheet()->rowFormats()->isHidden(row, &lastRow)) {
                for (int i = row; i <= lastRow; ++i)
                    listInt.append(i);
            }
            row = lastRow+1;
        }
        for (QList<int>::ConstIterator it = listInt.constBegin(); it != listInt.constEnd(); ++it)
            listRow += i18n("Row: %1", text.setNum(*it));

        list->addItems(listRow);
    }

    if (!list->count())
        enableButtonOk(false);

    //selection multiple
    list->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(this, &KoDialog::okClicked, this, &ShowColRow::slotOk);
    connect(list, &QListWidget::itemDoubleClicked, this, &ShowColRow::slotDoubleClicked);
    resize(200, 150);
    setFocus();
}

void ShowColRow::slotDoubleClicked(QListWidgetItem *)
{
    slotOk();
}

void ShowColRow::slotOk()
{
    Region region;
    for (unsigned int i = 0; i < (unsigned int)list->count(); i++) {
        if (list->item(i)->isSelected()) {
            if (typeShow == Column) {
                region.add(QRect(listInt.at(i), 1, 1, KS_rowMax));
            }
            if (typeShow == Row) {
                region.add(QRect(1, listInt.at(i), KS_colMax, 1));
            }
        }
    }

    HideShowManipulator* manipulator = new HideShowManipulator();
    manipulator->setSheet(m_selection->activeSheet());
    if (typeShow == Column) {
        manipulator->setManipulateColumns(true);
    }
    if (typeShow == Row) {
        manipulator->setManipulateRows(true);
    }
    manipulator->setReverse(true);
    manipulator->add(region);
    manipulator->execute(m_selection->canvas());

    accept();
}
