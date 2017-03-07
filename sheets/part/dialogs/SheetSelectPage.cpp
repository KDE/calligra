/* This file is part of the KDE project
   Copyright (C) 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

// Local
#include "SheetSelectPage.h"

// Calligra
#include <KoIcon.h>

// Sheets
#include "SheetsDebug.h"

using namespace Calligra::Sheets;

SheetSelectPage::SheetSelectPage(QWidget *parent)
        : QWidget(parent)
{
    m_ui.setupUi(this);
    setWindowTitle(i18n("Sheets"));

    //disabling automated sorting
    m_ui.ListViewAvailable->setSortingEnabled(false);
    m_ui.ListViewSelected->setSortingEnabled(false);

    //setup icons
    m_ui.ButtonSelectAll->setIcon(koIcon("go-last"));
    m_ui.ButtonSelect->setIcon(koIcon("go-next"));
    m_ui.ButtonRemove->setIcon(koIcon("go-previous"));
    m_ui.ButtonRemoveAll->setIcon(koIcon("go-first"));

    m_ui.ButtonMoveTop->setIcon(koIcon("go-top"));
    m_ui.ButtonMoveUp->setIcon(koIcon("go-up"));
    m_ui.ButtonMoveDown->setIcon(koIcon("go-down"));
    m_ui.ButtonMoveBottom->setIcon(koIcon("go-bottom"));

    //connect buttons
    connect(m_ui.ButtonSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(m_ui.ButtonSelect, SIGNAL(clicked()), this, SLOT(select()));
    connect(m_ui.ButtonRemove, SIGNAL(clicked()), this, SLOT(remove()));
    connect(m_ui.ButtonRemoveAll, SIGNAL(clicked()), this, SLOT(removeAll()));

    connect(m_ui.ButtonMoveTop, SIGNAL(clicked()), this, SLOT(moveTop()));
    connect(m_ui.ButtonMoveUp, SIGNAL(clicked()), this, SLOT(moveUp()));
    connect(m_ui.ButtonMoveDown, SIGNAL(clicked()), this, SLOT(moveDown()));
    connect(m_ui.ButtonMoveBottom, SIGNAL(clicked()), this, SLOT(moveBottom()));

    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonSelectAll, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonSelect, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonRemove, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonRemoveAll, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonMoveTop, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonMoveUp, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonMoveDown, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ButtonMoveBottom, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ListViewAvailable, SLOT(setEnabled(bool)));
    connect(m_ui.selectedSheetsButton, SIGNAL(toggled(bool)), m_ui.ListViewSelected, SLOT(setEnabled(bool)));
}

// SheetSelectPage::~SheetSelectPage()
// {
// }

/*
void SheetSelectPage::getOptions( QMap<QString,QString>& opts, bool incldef )
{
  QStringList sheetlist = this->selectedSheets();
  QStringList::iterator it;
  unsigned int i = 0;
  for (it = sheetlist.begin(); it != sheetlist.end(); ++it, i++)
  {
    opts.insert(printOptionForIndex(i),*it);
  }
}

void SheetSelectPage::setOptions( const QMap<QString,QString>& opts )
{
  unsigned int i = 0;
  QStringList sheetlist;
  while (opts.contains(printOptionForIndex(i)))
  {
    sheetlist.prepend(opts[printOptionForIndex(i++)]);
  }

  QStringList::iterator it;
  for (it = sheetlist.begin(); it != sheetlist.end(); ++it)
  {
    debugSheets <<" adding sheet to list of printed sheets:" << *it;
    this->prependSelectedSheet(*it);
  }
}
*/

void SheetSelectPage::setAvailableSheets(const QStringList& sheetlist)
{
    foreach(const QString &sname, sheetlist) {
        debugSheets << " adding sheet to list of printed sheets:" << sname;
        this->prependSelectedSheet(sname);
    }

}

bool SheetSelectPage::isValid(QString& /*msg*/)
{
    // we print the activeSheet() by default if no sheet is selected,
    // so we return true in any case

//   if (ListViewSelected->childCount() < 1)
//   {
//     msg = i18n("No sheets selected for printing!");
//     return false;
//   }
    return true;
}

QString SheetSelectPage::printOptionForIndex(unsigned int index)
{
    return QString("sheetprintorder%1").arg(index);
}

void SheetSelectPage::prependAvailableSheet(const QString& sheetname)
{
    m_ui.ListViewAvailable->insertItem(0, sheetname);
}

void SheetSelectPage::prependSelectedSheet(const QString& sheetname)
{
    m_ui.ListViewSelected->insertItem(0, sheetname);
}

QStringList SheetSelectPage::selectedSheets()
{
    QStringList list;
    for (int row = 0; row < m_ui.ListViewSelected->count(); ++row)
        list.append(m_ui.ListViewSelected->item(row)->text());
    return list;
}

/*
QStringList SheetSelectPage::selectedSheets(QPrinter &prt)
{
  QStringList list;
  unsigned int index;
  const QMap<QString,QString>& options = prt.options();
  for (index = 0; options.contains(SheetSelectPage::printOptionForIndex(index)); index++)
  {
    list.append(options[SheetSelectPage::printOptionForIndex(index)]);
  }
  return list;
}
*/

void SheetSelectPage::clearSelection()
{
    m_ui.ListViewSelected->clear();
}

void SheetSelectPage::selectAll()
{
    //we have to add all the stuff in reverse order
    // because inserted items (prependSelectedSheet) are prepended
    for (int row = m_ui.ListViewAvailable->count() - 1; row >= 0; --row)
        this->prependSelectedSheet(m_ui.ListViewAvailable->item(row)->text());
}

void SheetSelectPage::select()
{
    //we have to add all the stuff in reverse order
    // because inserted items (prependSelectedSheet) are prepended
    for (int row = m_ui.ListViewAvailable->count() - 1; row >= 0; --row)
        if (m_ui.ListViewAvailable->item(row)->isSelected())
            this->prependSelectedSheet(m_ui.ListViewAvailable->item(row)->text());
}

void SheetSelectPage::remove()
{
    for (int row = 0; row < m_ui.ListViewSelected->count();) {
        if (m_ui.ListViewSelected->item(row)->isSelected())
            delete m_ui.ListViewSelected->takeItem(row);
        else
            row++;
    }
}

void SheetSelectPage::removeAll()
{
    m_ui.ListViewSelected->clear();
}


void SheetSelectPage::moveTop()
{
    // moves the selected item to the top of the list
    QListWidgetItem* item = m_ui.ListViewSelected->takeItem(m_ui.ListViewSelected->currentRow());
    m_ui.ListViewSelected->insertItem(0, item);
    m_ui.ListViewSelected->setCurrentItem(item);
}

void SheetSelectPage::moveUp()
{
    // moves the selected item up one row
    int row = m_ui.ListViewSelected->currentRow();
    if (row > 0) {
        QListWidgetItem* item = m_ui.ListViewSelected->takeItem(row);
        m_ui.ListViewSelected->insertItem(row - 1, item);
        m_ui.ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveDown()
{
    // moves the selected item down one row
    int row = m_ui.ListViewSelected->currentRow();
    if (row < m_ui.ListViewSelected->count() - 1) {
        QListWidgetItem* item = m_ui.ListViewSelected->takeItem(row);
        m_ui.ListViewSelected->insertItem(row + 1, item);
        m_ui.ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveBottom()
{
    // moves the selected item to the bottom of the list
    QListWidgetItem* item = m_ui.ListViewSelected->takeItem(m_ui.ListViewSelected->currentRow());
    m_ui.ListViewSelected->addItem(item);
    m_ui.ListViewSelected->setCurrentItem(item);
}
