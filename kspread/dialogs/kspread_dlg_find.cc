/* This file is part of the KDE project
   Copyright (C) 1999-2005 Laurent Montel <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/


#include "kspread_dlg_find.h"
#include <QCheckBox>
#include <QLayout>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <klocale.h>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

using namespace KSpread;

FindOption::FindOption( QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    m_moreOptions = new QPushButton( i18n( "More Options" ), parent );
    layout->addWidget(  m_moreOptions );

    connect( m_moreOptions, SIGNAL( clicked () ), this, SLOT( slotMoreOptions() ) );

    m_findExtension = new QWidget( parent );
    layout->addWidget( m_findExtension );
    QVBoxLayout *layout1 = new QVBoxLayout( m_findExtension );
    m_searchInAllSheet = new QCheckBox( i18n( "Search entire sheet" ),m_findExtension );
    layout1->addWidget( m_searchInAllSheet );

    QHBoxLayout *comboLayout = new QHBoxLayout( m_findExtension );
    QLabel *label = new QLabel( i18n( "Search in:" ), m_findExtension );
    comboLayout->addWidget( label );

    m_searchIn = new QComboBox( m_findExtension );
    comboLayout->addWidget( m_searchIn );
    layout1->addLayout( comboLayout );

    QStringList lst;
    lst << i18n( "Cell Values" );
    lst << i18n( "Comments" );
    m_searchIn->insertItems( 0, lst );

    comboLayout = new QHBoxLayout( m_findExtension );
    label = new QLabel( i18n( "Search direction:" ), m_findExtension );
    comboLayout->addWidget( label );

    m_searchDirection = new QComboBox( m_findExtension );
    comboLayout->addWidget( m_searchDirection );
    layout1->addLayout( comboLayout );

    lst.clear();
    lst << i18n( "Across then Down" );
    lst << i18n( "Down then Across" );
    m_searchDirection->insertItems( 0, lst );

    m_findExtension->hide();
    emit adjustSize();
}

FindOption::searchTypeValue FindOption::searchType() const
{
    int pos = m_searchIn->currentIndex();
    if ( pos == 0 )
        return Value;
    else if ( pos == 1 )
        return Note;
    else
        return Value;
}

FindOption::searchDirectionValue FindOption::searchDirection() const
{
    int pos = m_searchDirection->currentIndex();
    if ( pos == 0 )
        return Row;
    else if ( pos == 1 )
        return Column;
    else
        return Row;
}


void FindOption::slotMoreOptions()
{
    if ( m_findExtension->isHidden() )
    {
        m_findExtension->show();
        m_moreOptions->setText( i18n( "Fewer Options" ));
    }
    else
    {
        m_findExtension->hide();
        m_moreOptions->setText( i18n( "More Options" ));
    }
    emit adjustSize();
}

bool FindOption::searchInAllSheet() const
{
    return m_searchInAllSheet->isChecked();
}

FindDlg::FindDlg(QWidget *parent, const char *name, long options, const QStringList &findStrings, bool hasSelection )
    : KFindDialog(parent,name,options,findStrings,hasSelection  )
{
    m_findOptions = new FindOption( findExtension() );
    connect( m_findOptions, SIGNAL( adjustSize() ), SLOT( slotAjustSize() ) );
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

FindDlg::~FindDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void FindDlg::slotAjustSize()
{
    adjustSize();
}

bool FindDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}


SearchDlg::SearchDlg(QWidget *parent, const char *name, long options, const QStringList &findStrings, const QStringList &replaceStrings, bool hasSelection )
    : KReplaceDialog(parent,name,options,findStrings,replaceStrings,hasSelection  )
{
    m_findOptions = new FindOption( findExtension() );
    connect( m_findOptions, SIGNAL( adjustSize() ), SLOT( slotAjustSize() ) );
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

SearchDlg::~SearchDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void SearchDlg::slotAjustSize()
{
    adjustSize();
    setFixedSize(size());
}

bool SearchDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}


#include "kspread_dlg_find.moc"
