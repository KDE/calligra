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

// Calligra includes
#include <KoIcon.h>
// KSpread includes

using namespace Calligra::Sheets;

SheetSelectPage::SheetSelectPage(QWidget *parent)
        : QWidget(parent)
        , m_widget(new Ui::SheetSelectWidget())
{
    m_widget->setupUi(this);
    setWindowTitle(i18n("Sheets"));

    //disabling automated sorting
    m_widget->ListViewAvailable->setSortingEnabled(false);
    m_widget->ListViewSelected->setSortingEnabled(false);

    //setup icons
    m_widget->ButtonSelectAll->setIcon(koIcon("go-last"));
    m_widget->ButtonSelect->setIcon(koIcon("go-next"));
    m_widget->ButtonRemove->setIcon(koIcon("go-previous"));
    m_widget->ButtonRemoveAll->setIcon(koIcon("go-first"));

    m_widget->ButtonMoveTop->setIcon(koIcon("go-top"));
    m_widget->ButtonMoveUp->setIcon(koIcon("go-up"));
    m_widget->ButtonMoveDown->setIcon(koIcon("go-down"));
    m_widget->ButtonMoveBottom->setIcon(koIcon("go-bottom"));

    //connect buttons
    connect(m_widget->ButtonSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(m_widget->ButtonSelect, SIGNAL(clicked()), this, SLOT(select()));
    connect(m_widget->ButtonRemove, SIGNAL(clicked()), this, SLOT(remove()));
    connect(m_widget->ButtonRemoveAll, SIGNAL(clicked()), this, SLOT(removeAll()));

    connect(m_widget->ButtonMoveTop, SIGNAL(clicked()), this, SLOT(moveTop()));
    connect(m_widget->ButtonMoveUp, SIGNAL(clicked()), this, SLOT(moveUp()));
    connect(m_widget->ButtonMoveDown, SIGNAL(clicked()), this, SLOT(moveDown()));
    connect(m_widget->ButtonMoveBottom, SIGNAL(clicked()), this, SLOT(moveBottom()));

    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonSelectAll, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonSelect, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonRemove, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonRemoveAll, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonMoveTop, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonMoveUp, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonMoveDown, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ButtonMoveBottom, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ListViewAvailable, SLOT(setEnabled(bool)));
    connect(m_widget->selectedSheetsButton, SIGNAL(toggled(bool)), m_widget->ListViewSelected, SLOT(setEnabled(bool)));
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
    foreach(const QString &sname, sheetlist) {
        kDebug() << " adding sheet to list of printed sheets:" << sname;
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
    m_widget->ListViewAvailable->insertItem(0, sheetname);
}

void SheetSelectPage::prependSelectedSheet(const QString& sheetname)
{
    m_widget->ListViewSelected->insertItem(0, sheetname);
}

QStringList SheetSelectPage::selectedSheets()
{
    QStringList list;
    for (int row = 0; row < m_widget->ListViewSelected->count(); ++row)
        list.append(m_widget->ListViewSelected->item(row)->text());
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
    m_widget->ListViewSelected->clear();
}

void SheetSelectPage::selectAll()
{
    //we have to add all the stuff in reverse order
    // because inserted items (prependSelectedSheet) are prepended
    for (int row = m_widget->ListViewAvailable->count() - 1; row >= 0; --row)
        this->prependSelectedSheet(m_widget->ListViewAvailable->item(row)->text());
}

void SheetSelectPage::select()
{
    //we have to add all the stuff in reverse order
    // because inserted items (prependSelectedSheet) are prepended
    for (int row = m_widget->ListViewAvailable->count() - 1; row >= 0; --row)
        if (m_widget->ListViewAvailable->item(row)->isSelected())
            this->prependSelectedSheet(m_widget->ListViewAvailable->item(row)->text());
}

void SheetSelectPage::remove()
{
    for (int row = 0; row < m_widget->ListViewSelected->count();) {
        if (m_widget->ListViewSelected->item(row)->isSelected())
            delete m_widget->ListViewSelected->takeItem(row);
        else
            row++;
    }
}

void SheetSelectPage::removeAll()
{
    m_widget->ListViewSelected->clear();
}


void SheetSelectPage::moveTop()
{
    // moves the selected item to the top of the list
    QListWidgetItem* item = m_widget->ListViewSelected->takeItem(m_widget->ListViewSelected->currentRow());
    m_widget->ListViewSelected->insertItem(0, item);
    m_widget->ListViewSelected->setCurrentItem(item);
}

void SheetSelectPage::moveUp()
{
    // moves the selected item up one row
    int row = m_widget->ListViewSelected->currentRow();
    if (row > 0) {
        QListWidgetItem* item = m_widget->ListViewSelected->takeItem(row);
        m_widget->ListViewSelected->insertItem(row - 1, item);
        m_widget->ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveDown()
{
    // moves the selected item down one row
    int row = m_widget->ListViewSelected->currentRow();
    if (row < m_widget->ListViewSelected->count() - 1) {
        QListWidgetItem* item = m_widget->ListViewSelected->takeItem(row);
        m_widget->ListViewSelected->insertItem(row + 1, item);
        m_widget->ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveBottom()
{
    // moves the selected item to the bottom of the list
    QListWidgetItem* item = m_widget->ListViewSelected->takeItem(m_widget->ListViewSelected->currentRow());
    m_widget->ListViewSelected->addItem(item);
    m_widget->ListViewSelected->setCurrentItem(item);
}

#include "SheetSelectPage.moc"
