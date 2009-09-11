/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptaccountsviewconfigdialog.h"

#include "kptaccountsview.h"
#include "kptaccountsmodel.h"

#include <QCheckBox>
#include <qdatetime.h>
#include <QString>

#include <kdatewidget.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

AccountsviewConfigDialog::AccountsviewConfigDialog( AccountsTreeView *view, QWidget *p)
    : KDialog(p),
    m_view( view )
{
    setCaption( i18n("Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new AccountsviewConfigPanel(this);
    switch ( view->startMode() ) {
        case CostBreakdownItemModel::StartMode_Project: 
            m_panel->ui_projectstartBtn->setChecked( true );
            m_panel->ui_startdate->setEnabled( false );
            break;
        case CostBreakdownItemModel::StartMode_Date:
            m_panel->ui_startdateBtn->setChecked( true );
            break;
    }
    switch ( view->endMode() ) {
        case CostBreakdownItemModel::EndMode_Project:
            m_panel->ui_projectendBtn->setChecked( true );
            m_panel->ui_enddate->setEnabled( false );
            break;
        case CostBreakdownItemModel::EndMode_Date:
            m_panel->ui_enddateBtn->setChecked( true );
            break;
        case CostBreakdownItemModel::EndMode_CurrentDate:
            m_panel->ui_currentdateBtn->setChecked( true );
            m_panel->ui_enddate->setEnabled( false );
            break;
    }
    m_panel->ui_startdate->setDate( view->startDate() );
    m_panel->ui_enddate->setDate( view->endDate() );
    m_panel->ui_periodBox->setCurrentIndex( view->periodType() );
    m_panel->ui_cumulative->setChecked( view->cumulative() );
    m_panel->ui_showBox->setCurrentIndex( view->showMode() );
    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(m_panel, SIGNAL(changed(bool)), SLOT( enableButtonOk(bool)));
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}


void AccountsviewConfigDialog::slotOk()
{
    kDebug();
    m_view->setPeriodType( m_panel->ui_periodBox->currentIndex() );
    m_view->setCumulative( m_panel->ui_cumulative->isChecked() );
    m_view->setShowMode( m_panel->ui_showBox->currentIndex() );
    if ( m_panel->ui_startdateBtn->isChecked() ) {
        m_view->setStartDate( m_panel->ui_startdate->date() );
        m_view->setStartMode( CostBreakdownItemModel::StartMode_Date );
    } else {
        m_view->setStartMode( CostBreakdownItemModel::StartMode_Project );
    }

    if ( m_panel->ui_enddateBtn->isChecked() ) {
        m_view->setEndDate( m_panel->ui_enddate->date() );
        m_view->setEndMode( CostBreakdownItemModel::EndMode_Date );
    } else if ( m_panel->ui_currentdateBtn->isChecked() ) {
        m_view->setEndMode( CostBreakdownItemModel::EndMode_CurrentDate );
    } else {
        m_view->setEndMode( CostBreakdownItemModel::EndMode_Project );
    }
}


//----------------------------
AccountsviewConfigPanel::AccountsviewConfigPanel(QWidget *parent)
    : AccountsviewConfigurePanelBase(parent)
{

    connect(ui_startdate, SIGNAL(dateChanged(const QDate&)), SLOT(slotChanged()));
    connect(ui_enddate, SIGNAL(dateChanged(const QDate&)), SLOT(slotChanged()));
    connect(ui_periodBox, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(ui_cumulative, SIGNAL(clicked()), SLOT(slotChanged()));

    connect(ui_projectstartBtn, SIGNAL(clicked()), SLOT(slotChanged()));
    connect(ui_startdateBtn, SIGNAL(clicked()), SLOT(slotChanged()));
    connect(ui_projectendBtn, SIGNAL(clicked()), SLOT(slotChanged()));
    connect(ui_currentdateBtn, SIGNAL(clicked()), SLOT(slotChanged()));
    connect(ui_enddateBtn, SIGNAL(clicked()), SLOT(slotChanged()));
    connect(ui_showBox, SIGNAL(activated(int)), SLOT(slotChanged()));
    
    connect(ui_startdateBtn, SIGNAL(toggled(bool)), ui_startdate, SLOT(setEnabled(bool)));
    connect(ui_enddateBtn, SIGNAL(toggled(bool)), ui_enddate, SLOT(setEnabled(bool)));
}

void AccountsviewConfigPanel::slotChanged() {
    emit changed(true);
}


}  //KPlato namespace

#include "kptaccountsviewconfigdialog.moc"
