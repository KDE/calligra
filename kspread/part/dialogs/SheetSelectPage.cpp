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

// standard C/C++ includes
// Qt includes
// KDE includes
#include <kdebug.h>

// KOffice includes
// KSpread includes

using namespace KSpread;

SheetSelectPage::SheetSelectPage( QWidget *parent )
    : QWidget(parent)
{
  setupUi(this);
  setWindowTitle(i18n("Sheets"));

  //disabling automated sorting
  ListViewAvailable->setSortingEnabled(false);
  ListViewSelected->setSortingEnabled(false);

  //setup icons
  ButtonSelectAll->setIcon(KIcon("go-last"));
  ButtonSelect->setIcon(KIcon("go-next"));
  ButtonRemove->setIcon(KIcon("go-previous"));
  ButtonRemoveAll->setIcon(KIcon("go-first"));

  ButtonMoveTop->setIcon(KIcon("go-top"));
  ButtonMoveUp->setIcon(KIcon("go-up"));
  ButtonMoveDown->setIcon(KIcon("go-down"));
  ButtonMoveBottom->setIcon(KIcon("go-bottom"));

  //connect buttons
  connect(ButtonSelectAll,SIGNAL(clicked()),this,SLOT(selectAll()));
  connect(ButtonSelect,SIGNAL(clicked()),this,SLOT(select()));
  connect(ButtonRemove,SIGNAL(clicked()),this,SLOT(remove()));
  connect(ButtonRemoveAll,SIGNAL(clicked()),this,SLOT(removeAll()));

  connect(ButtonMoveTop,SIGNAL(clicked()),this,SLOT(moveTop()));
  connect(ButtonMoveUp,SIGNAL(clicked()),this,SLOT(moveUp()));
  connect(ButtonMoveDown,SIGNAL(clicked()),this,SLOT(moveDown()));
  connect(ButtonMoveBottom,SIGNAL(clicked()),this,SLOT(moveBottom()));

  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonSelectAll, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonSelect, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonRemove, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonRemoveAll, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonMoveTop, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonMoveUp, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonMoveDown, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ButtonMoveBottom, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ListViewAvailable, SLOT(setEnabled(bool)));
  connect(selectedSheetsButton, SIGNAL(toggled(bool)), ListViewSelected, SLOT(setEnabled(bool)));
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
    kDebug() <<" adding sheet to list of printed sheets:" << *it;
    this->prependSelectedSheet(*it);
  }
}
*/

void SheetSelectPage::setAvailableSheets(const QStringList& sheetlist)
{
  foreach(QString sname, sheetlist)
  {
    kDebug() <<" adding sheet to list of printed sheets:" << sname;
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
  ListViewAvailable->insertItem(0, sheetname);
}

void SheetSelectPage::prependSelectedSheet(const QString& sheetname)
{
  ListViewSelected->insertItem(0, sheetname);
}

QStringList SheetSelectPage::selectedSheets()
{
  QStringList list;
  for (int row = 0; row < ListViewSelected->count(); ++row)
    list.append(ListViewSelected->item(row)->text());
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
  ListViewSelected->clear();
}

void SheetSelectPage::selectAll()
{
  //we have to add all the stuff in reverse order
  // because inserted items (prependSelectedSheet) are prepended
  for (int row = ListViewAvailable->count()-1; row >= 0; --row)
    this->prependSelectedSheet(ListViewAvailable->item(row)->text());
}

void SheetSelectPage::select()
{
  //we have to add all the stuff in reverse order
  // because inserted items (prependSelectedSheet) are prepended
  for (int row = ListViewAvailable->count()-1; row >= 0; --row)
    if (ListViewAvailable->item(row)->isSelected())
      this->prependSelectedSheet(ListViewAvailable->item(row)->text());
}

void SheetSelectPage::remove()
{
  for (int row = 0; row < ListViewSelected->count();)
  {
    if (ListViewSelected->item(row)->isSelected())
      delete ListViewSelected->takeItem(row);
    else
      row++;
  }
}

void SheetSelectPage::removeAll()
{
  ListViewSelected->clear();
}


void SheetSelectPage::moveTop()
{
  // moves the selected item to the top of the list
  QListWidgetItem* item = ListViewSelected->takeItem(ListViewSelected->currentRow());
  ListViewSelected->insertItem(0, item);
  ListViewSelected->setCurrentItem(item);
}

void SheetSelectPage::moveUp()
{
  // moves the selected item up one row
  int row = ListViewSelected->currentRow();
  if (row > 0) {
    QListWidgetItem* item = ListViewSelected->takeItem(row);
    ListViewSelected->insertItem(row-1, item);
    ListViewSelected->setCurrentItem(item);
  }
}

void SheetSelectPage::moveDown()
{
  // moves the selected item down one row
  int row = ListViewSelected->currentRow();
  if (row < ListViewSelected->count()-1) {
    QListWidgetItem* item = ListViewSelected->takeItem(row);
    ListViewSelected->insertItem(row+1, item);
    ListViewSelected->setCurrentItem(item);
  }
}

void SheetSelectPage::moveBottom()
{
  // moves the selected item to the bottom of the list
  QListWidgetItem* item = ListViewSelected->takeItem(ListViewSelected->currentRow());
  ListViewSelected->addItem(item);
  ListViewSelected->setCurrentItem(item);
}

#include "SheetSelectPage.moc"
