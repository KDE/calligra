/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_dlg_sort.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include <qlayout.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlistbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qcombobox.h>

KSpreadSortDlg::KSpreadSortDlg( KSpreadView* parent, const char* name)
	: KDialogBase( parent, name, TRUE,i18n("Sort"),Ok|Cancel )
{
  m_pView = parent;
  QWidget * page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );

  QGridLayout *lay2 = new QGridLayout( lay1, 2, 2 );
  lay2->setSpacing( 15 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, 
                                        i18n("Sort by"), page);
  grp->setRadioButtonExclusive( true );
  grp->layout();
  lay2->addWidget(grp, 0, 0);

  m_rb_row = new QRadioButton( i18n("Row"), grp );
  m_rb_column = new QRadioButton( i18n("Column"), grp );

  m_combo = new QComboBox(page);
  lay2->addWidget(m_combo, 0, 1);

  m_decrease = new QCheckBox(i18n("Decrease mode"), page);
  lay2->addWidget(m_decrease, 1, 0);

  m_cpLayout = new QCheckBox(i18n("&Copy layout"), page);
  lay2->addWidget(m_cpLayout, 2, 0);

  init();

  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( grp, SIGNAL(pressed(int)), this,SLOT(slotpress(int)));
}

void KSpreadSortDlg::init()
{
    QRect r = m_pView->activeTable()-> selectionRect();

    // Entire columns selected ?
    if ( m_pView->activeTable()->isColumnSelected() )
    {
	m_rb_row->setEnabled(false);
	m_rb_column->setChecked(true);

	for (int i = r.left(); i <= r.right(); ++i)
	    m_list_column += i18n("Column %1").arg(util_encodeColumnLabelText(i));
    }
    // Entire rows selected ?
    else if ( m_pView->activeTable()->isRowSelected() )
    {
	m_rb_column->setEnabled(false);
	m_rb_row->setChecked(true);

	for (int i = r.top(); i <= r.bottom(); ++i)
	    m_list_row += i18n("Row %1").arg(i);
    }
    else
    {
	// Selection is only one row
	if ( r.top() == r.bottom() )
        {
	    m_rb_column->setEnabled(false);
	    m_rb_row->setChecked(true);
	}
	// only one column
	else if (r.left() == r.right())
        {
	    m_rb_row->setEnabled(false);
	    m_rb_column->setChecked(true);
	}
	else
        {
	    m_rb_column->setChecked(true);
	}

	for (int i = r.left(); i <= r.right(); ++i)
	    m_list_column += i18n("Column %1").arg(util_encodeColumnLabelText(i));

	for (int i = r.top(); i <= r.bottom(); ++i)
	    m_list_row += i18n("Row %1").arg(i);
    }

    // Initialize the combo box
    if ( m_rb_row->isChecked() )
	slotpress(0);
    else
	slotpress(1);
}

void KSpreadSortDlg::slotpress(int id)
{
    switch(id)
    {
    case 0 :
	m_combo->clear();
	m_combo->insertStringList(m_list_row);
	break;
    case 1 :
	m_combo->clear();
	m_combo->insertStringList(m_list_column);
	break;
    default :
	kdDebug(36001) << "Error in signal : pressed(int id)" << endl;
	break;
    }
}

void KSpreadSortDlg::slotOk()
{
    QRect r = m_pView->activeTable()-> selectionRect();

    if( m_rb_row->isChecked())
    {
	if (!m_decrease->isChecked())
        {
	    m_pView->activeTable()->sortByRow(m_combo->currentItem() + r.top(),
                                              KSpreadTable::Increase,
                                              m_cpLayout->isChecked());
	}
	else
        {
	    m_pView->activeTable()->sortByRow(m_combo->currentItem() + r.top(),
                                              KSpreadTable::Decrease,
                                              m_cpLayout->isChecked());
	}
    }
    else if (m_rb_column->isChecked())
    {
	if (!m_decrease->isChecked())
        {
	    m_pView->activeTable()->sortByColumn(m_combo->currentItem() + r.left(),
                                                 KSpreadTable::Increase,
                                                 m_cpLayout->isChecked());
	}
	else
        {
	    m_pView->activeTable()->sortByColumn(m_combo->currentItem() + r.left(),
                                                 KSpreadTable::Decrease,
                                                 m_cpLayout->isChecked());
	}
    }
    else
    {
	kdDebug(36001) << "Err in radiobutton" << endl;
    }

    accept();
}

#include "kspread_dlg_sort.moc"
