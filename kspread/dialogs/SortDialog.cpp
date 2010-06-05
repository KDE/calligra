/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2006 Tomas Mecir <mecirt@gmail.com>
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

// Local
#include "SortDialog.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QRect>
#include <KTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kvbox.h>

#include "Map.h"
#include "Selection.h"
#include "Sheet.h"

// commands
#include "commands/SortManipulator.h"

using namespace KSpread;

SortDialog::SortDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , m_selection(selection)
{
    setCaption(i18n("Sort"));
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    setObjectName("SortDialog");

    resize(528, 316);
    setWindowTitle(i18n("Sorting"));
    //setSizeGripEnabled( true );

    KVBox *page = new KVBox();
    setMainWidget(page);

    m_tabWidget = new KTabWidget(page);

    m_page1 = new QWidget(m_tabWidget);
    QGridLayout * page1Layout = new QGridLayout(m_page1);
    page1Layout->setMargin(KDialog::marginHint());
    page1Layout->setSpacing(KDialog::spacingHint());

//---------------- Sort Layout & Header Row/Column Toggle

    //Sort orientation selector (for selecting Left-To-Right or Top-To-Bottom sorting of the selection)
    QGroupBox* layoutGroup = new QGroupBox(m_page1);
    layoutGroup->setTitle(i18n("Layout"));
    QVBoxLayout * layoutGroupLayout = new QVBoxLayout(layoutGroup);

    QHBoxLayout * orientationGroup = new QHBoxLayout;

    orientationGroup->addWidget(m_sortRow = new QRadioButton(i18n("Sort &Rows")));

    orientationGroup->addWidget(m_sortColumn = new QRadioButton(i18n("Sort &Columns")));

    //First row / column contains header toggle
    m_firstRowOrColHeader = new QCheckBox(layoutGroup);
    //m_firstRowOrColHeader->setText( i18n( "&First row contains headers" ) );
    m_firstRowOrColHeader->setChecked(false);

    layoutGroupLayout->addLayout(orientationGroup);
    layoutGroupLayout->addWidget(m_firstRowOrColHeader);

    page1Layout->addWidget(layoutGroup, 0, 0);

//----------------

    page1Layout->addItem(new QSpacerItem(0, 10), 2, 0);

    QGroupBox * sort1Box = new QGroupBox(m_page1);
    sort1Box->setTitle(i18n("Sort By"));
    sort1Box->setFlat(true);
    QHBoxLayout * sort1BoxLayout = new QHBoxLayout(sort1Box);
    sort1BoxLayout->setSpacing(KDialog::spacingHint());
    sort1BoxLayout->setMargin(KDialog::marginHint());
    sort1BoxLayout->setAlignment(Qt::AlignTop);

    m_sortKey1 = new KComboBox(sort1Box);
    sort1BoxLayout->addWidget(m_sortKey1);

    m_sortOrder1 = new KComboBox(sort1Box);
    m_sortOrder1->insertItem(0, i18n("Ascending"));
    m_sortOrder1->insertItem(1, i18n("Descending"));
    sort1BoxLayout->addWidget(m_sortOrder1);

    page1Layout->addWidget(sort1Box, 3, 0);

    QGroupBox * sort2Box = new QGroupBox(m_page1);
    sort2Box->setTitle(i18n("Then By"));
    sort2Box->setFlat(true);
    QHBoxLayout * sort2BoxLayout = new QHBoxLayout(sort2Box);
    sort2BoxLayout->setSpacing(KDialog::spacingHint());
    sort2BoxLayout->setMargin(KDialog::marginHint());
    sort2BoxLayout->setAlignment(Qt::AlignTop);

    m_sortKey2 = new KComboBox(sort2Box);
    m_sortKey2->insertItem(0, i18n("None"));
    sort2BoxLayout->addWidget(m_sortKey2);

    m_sortOrder2 = new KComboBox(sort2Box);
    m_sortOrder2->insertItem(0, i18n("Ascending"));
    m_sortOrder2->insertItem(1, i18n("Descending"));
    sort2BoxLayout->addWidget(m_sortOrder2);

    page1Layout->addWidget(sort2Box, 4, 0);

    QGroupBox * sort3Box = new QGroupBox(m_page1);
    sort3Box->setTitle(i18n("Then By"));
    sort3Box->setFlat(true);
    QHBoxLayout * sort3BoxLayout = new QHBoxLayout(sort3Box);
    sort3BoxLayout->setSpacing(KDialog::spacingHint());
    sort3BoxLayout->setMargin(KDialog::marginHint());
    sort3BoxLayout->setAlignment(Qt::AlignTop);

    m_sortKey3 = new KComboBox(sort3Box);
    m_sortKey3->insertItem(0, i18n("None"));
    m_sortKey3->setEnabled(false);
    sort3BoxLayout->addWidget(m_sortKey3);

    m_sortOrder3 = new KComboBox(sort3Box);
    m_sortOrder3->insertItem(0, i18n("Ascending"));
    m_sortOrder3->insertItem(1, i18n("Descending"));
    m_sortOrder3->setEnabled(false);
    sort3BoxLayout->addWidget(m_sortOrder3);

    page1Layout->addWidget(sort3Box, 5, 0);
    m_tabWidget->insertTab(0, m_page1, i18n("Sort Criteria"));


    //---------------- options page

    m_page2 = new QWidget(m_tabWidget);
    QGridLayout * page2Layout = new QGridLayout(m_page2);
    page2Layout->setMargin(KDialog::marginHint());
    page2Layout->setSpacing(KDialog::spacingHint());
    page2Layout->setAlignment(Qt::AlignTop);

    QGroupBox * firstKeyBox = new QGroupBox(m_page2);
    firstKeyBox->setTitle(i18n("First Key"));
    QVBoxLayout * firstKeyBoxLayout = new QVBoxLayout(firstKeyBox);
    firstKeyBoxLayout->setSpacing(KDialog::spacingHint());
    firstKeyBoxLayout->setMargin(KDialog::marginHint());
    firstKeyBoxLayout->setAlignment(Qt::AlignTop);

    m_useCustomLists = new QCheckBox(firstKeyBox);
    m_useCustomLists->setText(i18n("&Use custom list"));
    firstKeyBoxLayout->addWidget(m_useCustomLists);

    m_customList = new KComboBox(firstKeyBox);
    m_customList->setEnabled(false);
    m_customList->setMaximumSize(230, 30);
    firstKeyBoxLayout->addWidget(m_customList);

    page2Layout->addWidget(firstKeyBox, 0, 0);


    /*
     This doesn't work properly, and as a bug report pointed out, it isn't that useful since it is easy
     to just copy and paste the data and then sort the newly pasted data in place.
     -- Robert Knight

     (Tomas) In addition, the Sort manipulator, which is now being used, doesn't
     support this at all. So unless someone feels like adding the feature, I will
     remove this.

     QGroupBox * resultToBox = new QGroupBox( m_page2 );
     resultToBox->setTitle( i18n( "Location to Store Sort Results" ) );
     QHBoxLayout * resultToBoxLayout = new QHBoxLayout( resultToBox );
     resultToBoxLayout->setSpacing( KDialog::spacingHint() );
     resultToBoxLayout->setMargin( KDialog::marginHint() );
     resultToBoxLayout->setAlignment( Qt::AlignTop );

     QLabel * destinationSheet=new QLabel(resultToBox,"destinationSheet");
     destinationSheet->setText(i18n("Destination Sheet:"));
     resultToBoxLayout->addWidget(destinationSheet);

     m_outputSheet = new KComboBox( resultToBox );
     resultToBoxLayout->addWidget( m_outputSheet );
     QSpacerItem * spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
     resultToBoxLayout->addItem( spacer );

     QLabel * startingCellLabel = new QLabel( resultToBox, "destinationCellLabel" );
     startingCellLabel->setText( i18n( "Destination Cell:" ) );
     resultToBoxLayout->addWidget( startingCellLabel );

     m_outputCell = new KLineEdit( resultToBox );
     m_outputCell->setMaximumSize( QSize( 60, 32767 ) );
     resultToBoxLayout->addWidget( m_outputCell );

     page2Layout->addWidget( resultToBox, 1,0 );*/


    m_tabWidget->insertTab(1, m_page2, i18n("Options"));

    m_copyLayout = new QCheckBox(m_page2);
    m_copyLayout->setText(i18n("Copy cell &formatting (Borders, Colors, Text Style)"));

    page2Layout->addWidget(m_copyLayout, 1, 0);

    m_respectCase = new QCheckBox(m_page2);
    m_respectCase->setText(i18n("Case sensitive sort"));
    m_respectCase->setChecked(false);

    page2Layout->addWidget(m_respectCase, 2, 0);

    connect(m_sortKey2, SIGNAL(activated(int)), this,
            SLOT(sortKey2textChanged(int)));
    connect(m_useCustomLists, SIGNAL(stateChanged(int)), this,
            SLOT(useCustomListsStateChanged(int)));
    connect(m_firstRowOrColHeader, SIGNAL(stateChanged(int)), this,
            SLOT(firstRowHeaderChanged(int)));
    connect(orientationGroup, SIGNAL(pressed(int)), this,
            SLOT(slotOrientationChanged(int)));
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));

    init();
}

QRect SortDialog::sourceArea()
{
    return m_selection->lastRange();
}

SortDialog::Orientation SortDialog::guessDataOrientation()
{
    const QRect selection = sourceArea();

    if (selection.width() >= selection.height())
        return SortColumns;
    else
        return SortRows;
}

SortDialog::~SortDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void SortDialog::init()
{
    QStringList lst;
    lst << i18n("January") + ',' + i18n("February") + ',' + i18n("March") +
    ',' + i18n("April") + ',' + i18n("May") + ',' + i18n("June") +
    ',' + i18n("July") + ',' + i18n("August") + ',' + i18n("September") +
    ',' + i18n("October") + ',' + i18n("November") +
    ',' + i18n("December");

    lst << i18n("Monday") + ',' + i18n("Tuesday") + ',' + i18n("Wednesday") +
    ',' + i18n("Thursday") + ',' + i18n("Friday") + ',' + i18n("Saturday") +
    ',' + i18n("Sunday");

    KSharedConfigPtr config = KGlobal::activeComponent().config();
    const QStringList other = config->group("Parameters").readEntry("Other list", QStringList());
    QString tmp;
    for (QStringList::ConstIterator it = other.begin(); it != other.end(); ++it) {
        if ((*it) != "\\")
            tmp += (*it) + ", ";
        else if (it != other.begin()) {
            tmp = tmp.left(tmp.length() - 2);
            lst.append(tmp);
            tmp = "";
        }
    }
    m_customList->insertItems(0, lst);

    /*QPtrList<Sheet> sheetList = m_pView->map()->sheetList();
    for (unsigned int c = 0; c < sheetList.count(); ++c)
    {
      Sheet * t = sheetList.at(c);
      if (!t)
        continue;
      m_outputSheet->insertItem( t->sheetName() );
    }
    m_outputSheet->setItemText( m_pView->activeSheet()->sheetName() );*/

    QRect r = sourceArea();
    /*QString cellArea;
    cellArea += Cell::columnName(r.left());
    cellArea += QString::number( r.top() );
    m_outputCell->setText( cellArea );*/

    //If the top-most row or left-most column all contain text items (as opposed to numbers, dates etc.)
    //then the dialog will guess that the top row (or left column) is a header.
    //The user can always change this if we get this wrong.
    bool selectionMayHaveHeader = true;

    // Entire columns selected ?
    if (Region::Range(r).isColumn()) {
        m_sortColumn->setEnabled(false);
        m_sortRow->setChecked(true);

        int right = r.right();
        for (int i = r.left(); i <= right; ++i) {
            QString guessName = m_selection->activeSheet()->guessColumnTitle(r, i);
            QString colName = i18n(" (Column %1)", Cell::columnName(i));

            if (!guessName.isEmpty()) {
                m_listColumn += guessName + colName;
            } else {
                m_listColumn += i18n("Column %1", Cell::columnName(i));

                if (i == r.left())
                    selectionMayHaveHeader = false;
            }
        }
        // m_listColumn += i18n("Column %1",Cell::columnName(i));
    }
    // Entire rows selected ?
    else if (Region::Range(r).isRow()) {
        m_sortRow->setEnabled(false);
        m_sortColumn->setChecked(true);

        int bottom = r.bottom();
        for (int i = r.top(); i <= bottom; ++i) {
            QString guessName = m_selection->activeSheet()->guessRowTitle(r, i);
            QString rowName = i18n(" (Row %1)", i);

            if (!guessName.isEmpty()) {
                m_listRow += guessName + rowName;
            } else {
                m_listRow += i18n("Row %1", i);

                if (i == r.top())
                    selectionMayHaveHeader = false;
            }
        }
    } else {
        // Selection is only one row
        if (r.top() == r.bottom()) {
            m_sortRow->setEnabled(false);
            m_sortColumn->setChecked(true);
        }
        // only one column
        else if (r.left() == r.right()) {
            m_sortColumn->setEnabled(false);
            m_sortRow->setChecked(true);
        } else {
            if (guessDataOrientation() == SortColumns)
                m_sortColumn->setChecked(true);
            else
                m_sortRow->setChecked(true);
        }

        int right  = r.right();
        int bottom = r.bottom();
        for (int i = r.left(); i <= right; ++i) {
            QString guessName = m_selection->activeSheet()->guessColumnTitle(r, i);
            QString colName = i18n(" (Column %1)", Cell::columnName(i));

            if (!guessName.isEmpty())
                m_listColumn += guessName + colName;
            else {
                m_listColumn += i18n("Column %1", Cell::columnName(i));

                if (i == r.left())
                    selectionMayHaveHeader = false;
            }
        }

        for (int i = r.top(); i <= bottom; ++i) {
            QString guessName = m_selection->activeSheet()->guessRowTitle(r, i);
            QString rowName = i18n(" (Row %1)", i);

            if (!guessName.isEmpty())
                m_listRow += guessName + rowName;
            else {
                m_listRow += i18n("Row %1", i);

                if (i == r.top())
                    selectionMayHaveHeader = false;
            }
        }
    }

    if (selectionMayHaveHeader)
        m_firstRowOrColHeader->setChecked(true);
    else
        m_firstRowOrColHeader->setChecked(false);

    // Initialize the combo box
    if (m_sortRow->isChecked()) {
        slotOrientationChanged(SortColumns);
    } else {
        slotOrientationChanged(SortRows);
    }
}

void SortDialog::slotOrientationChanged(int id)
{
    switch (id) {
    case SortColumns :
        m_sortKey1->clear();
        m_sortKey2->clear();
        m_sortKey3->clear();
        m_sortKey1->insertItems(0, m_listColumn);
        m_sortKey2->insertItem(0, i18n("None"));
        m_sortKey2->insertItems(1, m_listColumn);
        m_sortKey3->insertItem(0, i18n("None"));
        m_sortKey3->insertItems(1, m_listColumn);
        m_firstRowOrColHeader->setText(i18n("&First row contains headers"));
        break;

    case SortRows :
        m_sortKey1->clear();
        m_sortKey2->clear();
        m_sortKey3->clear();
        m_sortKey1->insertItems(0, m_listRow);
        m_sortKey2->insertItem(0, i18n("None"));
        m_sortKey2->insertItems(1, m_listRow);
        m_sortKey3->insertItem(0, i18n("None"));
        m_sortKey3->insertItems(1, m_listRow);
        m_firstRowOrColHeader->setText(i18n("&First column contains headers"));

        /*if (m_firstRowOrColHeader->isChecked())
        {
          int k1 = m_sortKey1->currentIndex();
          int k2 = m_sortKey2->currentIndex();
          int k3 = m_sortKey3->currentIndex();
          m_sortKey1->removeItem( 0 );
          m_sortKey2->removeItem( 1 ); // because there is "None" in there
          m_sortKey3->removeItem( 1 );
          if (k1 > 0)
            m_sortKey1->setCurrentItem(--k1);
          else
            m_sortKey1->setCurrentItem( 0 );
          if (k2 > 0)
            m_sortKey2->setCurrentItem(--k2);
          if (k3 > 0)
            m_sortKey3->setCurrentItem(--k3);
        }*/

        break;

    default :
        kDebug(36001) << "Error in signal : pressed(int id)";
        break;
    }
}

void SortDialog::slotOk()
{
    Sheet * sheet = m_selection->activeSheet();

    SortManipulator *sm = new SortManipulator();
    sm->setSheet(sheet);

    // set parameters
    sm->setSortRows(m_sortRow->isChecked());
    sm->setSkipFirst(m_firstRowOrColHeader->isChecked());
    sm->setCaseSensitive(m_respectCase->isChecked());
    sm->setCopyFormat(m_copyLayout->isChecked());

    // retrieve sorting order
    bool sortAsc1 = (m_sortOrder1->currentIndex() == 0);
    bool sortAsc2 = (m_sortOrder2->currentIndex() == 0);
    bool sortAsc3 = (m_sortOrder3->currentIndex() == 0);
    int sort1 = m_sortKey1->currentIndex();
    int sort2 = m_sortKey2->currentIndex() - 1;  // first one is None
    int sort3 = m_sortKey3->currentIndex() - 1;  // first one is None
    // remove duplicates
    if (sort1 == sort2) sort2 = -1;
    if (sort1 == sort3) sort3 = -1;
    if (sort2 == sort3) sort3 = -1;

    // set sorting order
    sm->addSortBy(sort1, sortAsc1);
    if (sort2 >= 0) sm->addSortBy(sort2, sortAsc2);
    if (sort3 >= 0) sm->addSortBy(sort3, sortAsc3);

    if (m_useCustomLists->isChecked()) {
        // add custom list if any
        QStringList clist;
        QString list = m_customList->currentText();
        QString tmp;
        int l = list.length();
        for (int i = 0; i < l; ++i) {
            if (list[i] == ',') {
                clist.append(tmp.trimmed());
                tmp = "";
            } else
                tmp += list[i];
        }

        sm->setUseCustomList(true);
        sm->setCustomList(clist);
    }
    sm->add(sourceArea());
    sm->execute(m_selection->canvas());

    m_selection->emitModified();
    accept();
}

void SortDialog::sortKey2textChanged(int i)
{
    m_sortKey3->setEnabled((i != 0));
    m_sortOrder3->setEnabled((i != 0));
}

void SortDialog::useCustomListsStateChanged(int state)
{
    if (state == 0)
        m_customList->setEnabled(false);
    else if (state == 2)
        m_customList->setEnabled(true);
}

void SortDialog::firstRowHeaderChanged(int /*state*/)
{
    /* if (m_sortColumn->isChecked())
       return;

     if (state == 0) // off
     {
       int k1 = m_sortKey1->currentIndex();
       int k2 = m_sortKey2->currentIndex();
       int k3 = m_sortKey3->currentIndex();
       m_sortKey1->clear();
       m_sortKey2->clear();
       m_sortKey3->clear();
       m_sortKey1->insertItems( 0, m_listRow );
       m_sortKey2->insertItem( i18n("None") );
       m_sortKey2->insertItems( 0, m_listRow );
       m_sortKey3->insertItem( i18n("None") );
       m_sortKey3->insertItems( 0, m_listRow );

       m_sortKey1->setCurrentItem(++k1);
       m_sortKey2->setCurrentItem(++k2);
       m_sortKey3->setCurrentItem(++k3);
     }
     else if (state == 2) // on
     {
       int k1 = m_sortKey1->currentIndex();
       int k2 = m_sortKey2->currentIndex();
       int k3 = m_sortKey3->currentIndex();
       m_sortKey1->removeItem( 0 );
       m_sortKey2->removeItem( 1 ); // because there is "None" in there
       m_sortKey3->removeItem( 1 );
       if (k1 > 0)
         m_sortKey1->setCurrentItem(--k1);
       if (k2 > 0)
         m_sortKey2->setCurrentItem(--k2);
       if (k3 > 0)
         m_sortKey3->setCurrentItem(--k3);
     }*/
}

#include "SortDialog.moc"
