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

#include <QCheckBox>
#include <QComboBox>
#include <q3datetimeedit.h>
#include <qdatetime.h>
#include <QString>

#include <kdatewidget.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

AccountsviewConfigDialog::AccountsviewConfigDialog(const QDate &date, int period,  const QStringList &periodTexts, bool cumulative, QWidget *p)
    : KDialogBase(Swallow, i18n("Settings"), Ok|Cancel, Ok, p, "Accountsview Settings Dialog", true, true)
{
    m_panel = new AccountsviewConfigPanel(this);
    m_panel->dateEdit->setDate(date);
    m_panel->periodBox->insertStringList(periodTexts);
    m_panel->periodBox->setCurrentItem(period);
    m_panel->cumulative->setChecked(cumulative);
    setMainWidget(m_panel);
    
    enableButtonOK(false);
    
    connect(m_panel, SIGNAL(changed(bool)), SLOT( enableButtonOK(bool)));    
}


QDate AccountsviewConfigDialog::date() {
    return m_panel->dateEdit->date();
}

int AccountsviewConfigDialog::period() {
    return m_panel->periodBox->currentItem();
}

QString AccountsviewConfigDialog::periodText() {
    return m_panel->periodBox->currentText();
}

bool AccountsviewConfigDialog::isCumulative() {
    return m_panel->cumulative->isChecked();
}


//----------------------------
AccountsviewConfigPanel::AccountsviewConfigPanel(QWidget *parent)
    : AccountsviewConfigurePanelBase(parent) {
    
    connect(dateEdit, SIGNAL(changed(QDate)), SLOT(slotChanged()));
    connect(periodBox, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(cumulative, SIGNAL(clicked()), SLOT(slotChanged()));
}

void AccountsviewConfigPanel::slotChanged() {
    emit changed(true);
}


}  //KPlato namespace

#include "kptaccountsviewconfigdialog.moc"
