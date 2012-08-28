/* This file is part of the KDE project
   Copyright (C) 2005, 2012 Dag Andersen <danders@get2net.dk>

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
#include "kptviewbase.h"
#include "kptdebug.h"

#include "KoPageLayoutWidget.h"

#include <QCheckBox>
#include <QDateTime>
#include <QString>

#include <kdatewidget.h>
#include <klocale.h>


namespace KPlato
{

AccountsviewConfigDialog::AccountsviewConfigDialog( ViewBase *view, AccountsTreeView *treeview, QWidget *p)
    : KPageDialog(p),
    m_view( view ),
    m_treeview( treeview )
{
    setCaption( i18n("Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new AccountsviewConfigPanel( this );
    switch ( treeview->startMode() ) {
        case CostBreakdownItemModel::StartMode_Project: 
            m_panel->ui_projectstartBtn->setChecked( true );
            m_panel->ui_startdate->setEnabled( false );
            break;
        case CostBreakdownItemModel::StartMode_Date:
            m_panel->ui_startdateBtn->setChecked( true );
            break;
    }
    switch ( treeview->endMode() ) {
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
    m_panel->ui_startdate->setDate( treeview->startDate() );
    m_panel->ui_enddate->setDate( treeview->endDate() );
    m_panel->ui_periodBox->setCurrentIndex( treeview->periodType() );
    m_panel->ui_cumulative->setChecked( treeview->cumulative() );
    m_panel->ui_showBox->setCurrentIndex( treeview->showMode() );

    KPageWidgetItem *page = addPage( m_panel, i18n( "General" ) );
    page->setHeader( i18n( "View Settings" ) );

    QTabWidget *tab = new QTabWidget();

    QWidget *w = ViewBase::createPageLayoutWidget( view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );

    m_headerfooter = ViewBase::createHeaderFooterWidget( view );
    m_headerfooter->setOptions( view->printingOptions() );
    tab->addTab( m_headerfooter, m_headerfooter->windowTitle() );

    page = addPage( tab, i18n( "Printing" ) );
    page->setHeader( i18n( "Printing Options" ) );

    connect( this, SIGNAL(okClicked()), this, SLOT(slotOk()));

    connect(m_panel, SIGNAL(changed(bool)), SLOT( enableButtonOk(bool)));
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}


void AccountsviewConfigDialog::slotOk()
{
    kDebug(planDbg());
    m_treeview->setPeriodType( m_panel->ui_periodBox->currentIndex() );
    m_treeview->setCumulative( m_panel->ui_cumulative->isChecked() );
    m_treeview->setShowMode( m_panel->ui_showBox->currentIndex() );
    if ( m_panel->ui_startdateBtn->isChecked() ) {
        m_treeview->setStartDate( m_panel->ui_startdate->date() );
        m_treeview->setStartMode( CostBreakdownItemModel::StartMode_Date );
    } else {
        m_treeview->setStartMode( CostBreakdownItemModel::StartMode_Project );
    }

    if ( m_panel->ui_enddateBtn->isChecked() ) {
        m_treeview->setEndDate( m_panel->ui_enddate->date() );
        m_treeview->setEndMode( CostBreakdownItemModel::EndMode_Date );
    } else if ( m_panel->ui_currentdateBtn->isChecked() ) {
        m_treeview->setEndMode( CostBreakdownItemModel::EndMode_CurrentDate );
    } else {
        m_treeview->setEndMode( CostBreakdownItemModel::EndMode_Project );
    }

    m_view->setPageLayout( m_pagelayout->pageLayout() );
    m_view->setPrintingOptions( m_headerfooter->options() );
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
