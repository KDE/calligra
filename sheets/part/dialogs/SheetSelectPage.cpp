/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetSelectPage.h"

#include <KoIcon.h>

#include "engine/SheetsDebug.h"

using namespace Calligra::Sheets;

SheetSelectPage::SheetSelectPage(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    setWindowTitle(i18n("Sheets"));

    // disabling automated sorting
    m_ui.ListViewAvailable->setSortingEnabled(false);
    m_ui.ListViewSelected->setSortingEnabled(false);

    // setup icons
    m_ui.ButtonSelectAll->setIcon(koIcon("go-last"));
    m_ui.ButtonSelect->setIcon(koIcon("go-next"));
    m_ui.ButtonRemove->setIcon(koIcon("go-previous"));
    m_ui.ButtonRemoveAll->setIcon(koIcon("go-first"));

    m_ui.ButtonMoveTop->setIcon(koIcon("go-top"));
    m_ui.ButtonMoveUp->setIcon(koIcon("go-up"));
    m_ui.ButtonMoveDown->setIcon(koIcon("go-down"));
    m_ui.ButtonMoveBottom->setIcon(koIcon("go-bottom"));

    // connect buttons
    connect(m_ui.ButtonSelectAll, &QAbstractButton::clicked, this, &SheetSelectPage::selectAll);
    connect(m_ui.ButtonSelect, &QAbstractButton::clicked, this, &SheetSelectPage::select);
    connect(m_ui.ButtonRemove, &QAbstractButton::clicked, this, &SheetSelectPage::remove);
    connect(m_ui.ButtonRemoveAll, &QAbstractButton::clicked, this, &SheetSelectPage::removeAll);

    connect(m_ui.ButtonMoveTop, &QAbstractButton::clicked, this, &SheetSelectPage::moveTop);
    connect(m_ui.ButtonMoveUp, &QAbstractButton::clicked, this, &SheetSelectPage::moveUp);
    connect(m_ui.ButtonMoveDown, &QAbstractButton::clicked, this, &SheetSelectPage::moveDown);
    connect(m_ui.ButtonMoveBottom, &QAbstractButton::clicked, this, &SheetSelectPage::moveBottom);

    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonSelectAll, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonSelect, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonRemove, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonRemoveAll, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonMoveTop, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonMoveUp, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonMoveDown, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ButtonMoveBottom, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ListViewAvailable, &QWidget::setEnabled);
    connect(m_ui.selectedSheetsButton, &QAbstractButton::toggled, m_ui.ListViewSelected, &QWidget::setEnabled);
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

void SheetSelectPage::setAvailableSheets(const QStringList &sheetlist)
{
    foreach (const QString &sname, sheetlist) {
        debugSheets << " adding sheet to list of printed sheets:" << sname;
        this->prependSelectedSheet(sname);
    }
}

bool SheetSelectPage::isValid(QString & /*msg*/)
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

void SheetSelectPage::prependAvailableSheet(const QString &sheetname)
{
    m_ui.ListViewAvailable->insertItem(0, sheetname);
}

void SheetSelectPage::prependSelectedSheet(const QString &sheetname)
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
    // we have to add all the stuff in reverse order
    //  because inserted items (prependSelectedSheet) are prepended
    for (int row = m_ui.ListViewAvailable->count() - 1; row >= 0; --row)
        this->prependSelectedSheet(m_ui.ListViewAvailable->item(row)->text());
}

void SheetSelectPage::select()
{
    // we have to add all the stuff in reverse order
    //  because inserted items (prependSelectedSheet) are prepended
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
    QListWidgetItem *item = m_ui.ListViewSelected->takeItem(m_ui.ListViewSelected->currentRow());
    m_ui.ListViewSelected->insertItem(0, item);
    m_ui.ListViewSelected->setCurrentItem(item);
}

void SheetSelectPage::moveUp()
{
    // moves the selected item up one row
    int row = m_ui.ListViewSelected->currentRow();
    if (row > 0) {
        QListWidgetItem *item = m_ui.ListViewSelected->takeItem(row);
        m_ui.ListViewSelected->insertItem(row - 1, item);
        m_ui.ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveDown()
{
    // moves the selected item down one row
    int row = m_ui.ListViewSelected->currentRow();
    if (row < m_ui.ListViewSelected->count() - 1) {
        QListWidgetItem *item = m_ui.ListViewSelected->takeItem(row);
        m_ui.ListViewSelected->insertItem(row + 1, item);
        m_ui.ListViewSelected->setCurrentItem(item);
    }
}

void SheetSelectPage::moveBottom()
{
    // moves the selected item to the bottom of the list
    QListWidgetItem *item = m_ui.ListViewSelected->takeItem(m_ui.ListViewSelected->currentRow());
    m_ui.ListViewSelected->addItem(item);
    m_ui.ListViewSelected->setCurrentItem(item);
}
