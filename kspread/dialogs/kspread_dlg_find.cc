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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_dlg_find.h"
#include <qcheckbox.h>
#include <qlayout.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>

KSpreadFindOption::KSpreadFindOption( QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    m_moreOptions = new QPushButton( i18n( "More Options>>" ), parent );
    layout->addWidget(  m_moreOptions );

    connect( m_moreOptions, SIGNAL( clicked () ), this, SLOT( slotMoreOptions() ) );

    m_findExtension = new QWidget( parent );
    layout->addWidget( m_findExtension );
    QVBoxLayout *layout1 = new QVBoxLayout( m_findExtension );
    m_searchInAllSheet = new QCheckBox( i18n( "Search in All Sheet" ),m_findExtension );
    layout1->addWidget( m_searchInAllSheet );

    QHBoxLayout *comboLayout = new QHBoxLayout( m_findExtension );
    QLabel *label = new QLabel( i18n( "Search In:" ), m_findExtension );
    comboLayout->addWidget( label );

    m_searchIn = new QComboBox( m_findExtension );
    comboLayout->addWidget( m_searchIn );
    layout1->addLayout( comboLayout );

    QStringList lst;
    lst << i18n( "Value" );
    lst << i18n( "Notes" );
    m_searchIn->insertStringList( lst );

    m_findExtension->hide();
    emit adjustSize();
}

KSpreadFindOption::searchTypeValue KSpreadFindOption::searchType() const
{
    int pos = m_searchIn->currentItem();
    if ( pos == 0 )
        return Value;
    else if ( pos == 1 )
        return Note;
    else
        return Value;
}


void KSpreadFindOption::slotMoreOptions()
{
    if ( m_findExtension->isHidden() )
    {
        m_findExtension->show();
        m_moreOptions->setText( i18n( "More Options>>" ));
    }
    else
    {
        m_findExtension->hide();
        m_moreOptions->setText( i18n( "More Options <<" ));
    }
    emit adjustSize();
}

bool KSpreadFindOption::searchInAllSheet() const
{
    return m_searchInAllSheet->isChecked();
}

KSpreadFindDlg::KSpreadFindDlg(QWidget *parent, const char *name, long options, const QStringList &findStrings, bool hasSelection )
    : KFindDialog(parent,name,options,findStrings,hasSelection  )
{
    m_findOptions = new KSpreadFindOption( findExtension() );
    connect( m_findOptions, SIGNAL( adjustSize() ), SLOT( slotAjustSize() ) );
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

KSpreadFindDlg::~KSpreadFindDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void KSpreadFindDlg::slotAjustSize()
{
    adjustSize();
}

bool KSpreadFindDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}


KSpreadSearchDlg::KSpreadSearchDlg(QWidget *parent, const char *name, long options, const QStringList &findStrings, const QStringList &replaceStrings, bool hasSelection )
    : KReplaceDialog(parent,name,options,findStrings,replaceStrings,hasSelection  )
{
    m_findOptions = new KSpreadFindOption( findExtension() );
    connect( m_findOptions, SIGNAL( adjustSize() ), SLOT( slotAjustSize() ) );
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

KSpreadSearchDlg::~KSpreadSearchDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void KSpreadSearchDlg::slotAjustSize()
{
    adjustSize();
    setFixedSize(size());
}

bool KSpreadSearchDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}


#include "kspread_dlg_find.moc"
