/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#include "kptresourcedialog.h"
#include "kptproject.h"
#include "kptresource.h"

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>


KPTResourceDialogImpl::KPTResourceDialogImpl (QWidget *parent)
    : ResourceDialogBase(parent)
{

    connect(type, SIGNAL(activated(int)), SLOT(slotChanged(int)));
    connect(units, SIGNAL(valueChanged(int)), SLOT(slotChanged(int)));
    connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged(const QString&)));

    connect(fromEdit, SIGNAL(valueChanged(const QTime&)), SLOT(slotChanged(const QTime&)));
    connect(untilEdit, SIGNAL(valueChanged(const QTime&)), SLOT(slotChanged(const QTime&)));

    connect(rateEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged(const QString&)));
    connect(overtimeEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged(const QString&)));
    connect(fixedCostEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged(const QString&)));

    connect (chooseBtn, SIGNAL(pressed()), SLOT(slotChooseResource()));
}


void KPTResourceDialogImpl::slotChanged(const QString&) {
    emit changed();
}

void KPTResourceDialogImpl::slotChanged(const QTime&) {
    emit changed();
}

void KPTResourceDialogImpl::slotChanged(int) {
    emit changed();
}

void KPTResourceDialogImpl::slotCalculationNeeded(const QString&) {
    emit calculate();
    emit changed();
}

void KPTResourceDialogImpl::slotChooseResource()
{
  KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
  if (!a.isEmpty()) {
	  nameEdit->setText(a.fullEmail());
  }
}

//////////////////  KPTResourceDialog  ////////////////////////

KPTResourceDialog::KPTResourceDialog(KPTResource &resource, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Resource Settings"), Ok|Cancel, Ok, parent, name, true, true),
      m_resource(resource),
      m_calculationNeeded(false)
{
    dia = new KPTResourceDialogImpl(this);
    setMainWidget(dia);
    enableButtonOK(false);

    dia->nameEdit->setText(resource.name());
    dia->units->setValue(resource.units());
    dia->rateEdit->setText(KGlobal::locale()->formatMoney(resource.normalRate()));
    dia->overtimeEdit->setText(KGlobal::locale()->formatMoney(resource.overtimeRate()));
    dia->fixedCostEdit->setText(KGlobal::locale()->formatMoney(resource.fixedCost()));

    dia->fromEdit->setTime(resource.availableFrom());
    dia->untilEdit->setTime(resource.availableUntil());

    connect(dia, SIGNAL(changed()), SLOT(enableButtonOk()));
    connect(dia, SIGNAL(calculate()), SLOT(slotCalculationNeeded()));
}


void KPTResourceDialog::enableButtonOk() {
    enableButtonOK(true);
}

void KPTResourceDialog::slotCalculationNeeded() {
    m_calculationNeeded = true;
}

void KPTResourceDialog::slotOk() {
    if (dia->fromEdit->time() >= dia->untilEdit->time()) {
        KMessageBox::sorry(this, i18n("The specified 'from' time must be before 'until' time."));
        return;
    }

    m_resource.setName(dia->nameEdit->text());
    m_resource.setType((KPTResource::Type)(dia->type->currentItem()));
    m_resource.setUnits(dia->units->value());
    // TODO: check money format
    m_resource.setNormalRate(KGlobal::locale()->readMoney(dia->rateEdit->text()));
    m_resource.setOvertimeRate(KGlobal::locale()->readMoney(dia->overtimeEdit->text()));
    m_resource.setFixedCost(KGlobal::locale()->readMoney(dia->fixedCostEdit->text()));

    m_resource.setAvailableFrom(dia->fromEdit->time());
    m_resource.setAvailableUntil(dia->untilEdit->time());
    accept();
}

#include "kptresourcedialog.moc"
