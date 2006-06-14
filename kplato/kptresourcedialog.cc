/* This file is part of the KDE project
   Copyright (C) 2003 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptresourcedialog.h"
#include "kptcommand.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <q3datetimeedit.h>
#include <qdatetime.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <QSpinBox>
#include <q3ptrlist.h>
#include <qstringlist.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kcommand.h>
#include <kdatetimewidget.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

namespace KPlato
{

ResourceDialogImpl::ResourceDialogImpl (QWidget *parent)
    : ResourceDialogBase(parent)
{

    connect(type, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(units, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
    connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(initialsEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(emailEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));

    connect(calendarList, SIGNAL(activated(int)), SLOT(slotChanged()));

    connect(rateEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(overtimeEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));

    connect(chooseBtn, SIGNAL(clicked()), SLOT(slotChooseResource()));

    connect(availableFrom, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotChanged()));
    connect(availableUntil, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotChanged()));
    connect(availableFrom, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotAvailableFromChanged(const QDateTime&)));
    connect(availableUntil, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotAvailableUntilChanged(const QDateTime&)));
}


void ResourceDialogImpl::slotChanged() {
    emit changed();
}

void ResourceDialogImpl::slotAvailableFromChanged(const QDateTime&) {
    if (availableUntil->dateTime() < availableFrom->dateTime()) {
        disconnect(availableUntil, SIGNAL(valueChanged(const QDateTime&)), this,  SLOT(slotAvailableUntilChanged(const QDateTime&)));
        //kDebug()<<"From: "<<availableFrom->dateTime().toString()<<" until="<<availableUntil->dateTime().toString()<<endl;
        availableUntil->setDateTime(availableFrom->dateTime());
        connect(availableUntil, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotAvailableUntilChanged(const QDateTime&)));
    }
}

void ResourceDialogImpl::slotAvailableUntilChanged(const QDateTime&) {
    if (availableFrom->dateTime() > availableUntil->dateTime()) {
        disconnect(availableFrom, SIGNAL(valueChanged(const QDateTime&)), this,  SLOT(slotAvailableFromChanged(const QDateTime&)));
        //kDebug()<<"Until: "<<availableUntil->dateTime().toString()<<" from="<<availableFrom->dateTime().toString()<<endl;
        availableFrom->setDateTime(availableUntil->dateTime());
        connect(availableFrom, SIGNAL(valueChanged(const QDateTime&)), SLOT(slotAvailableFromChanged(const QDateTime&)));
    }
}

void ResourceDialogImpl::slotCalculationNeeded(const QString&) {
    emit calculate();
    emit changed();
}

void ResourceDialogImpl::slotChooseResource()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty()) {
        nameEdit->setText(a.assembledName());
        emailEdit->setText(a.preferredEmail());
        QStringList l = QStringList::split(' ', a.assembledName());
        QString in;
        QStringList::Iterator it = l.begin();
        for (/*int i = 0*/; it != l.end(); ++it) {
            in += (*it)[0];
        }
        initialsEdit->setText(in);
    }
}

//////////////////  ResourceDialog  ////////////////////////

ResourceDialog::ResourceDialog(Project &project, Resource *resource, QWidget *parent, const char *name)
    : KDialogBase( parent),
      m_original(resource),
      m_resource(resource),
      m_calculationNeeded(false)
{
    setCaption( i18n("Resource Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonSeparator( true );
    dia = new ResourceDialogImpl(this);
    setMainWidget(dia);
    enableButtonOK(false);

    dia->nameEdit->setText(resource->name());
    dia->initialsEdit->setText(resource->initials());
    dia->emailEdit->setText(resource->email());
    dia->type->setCurrentItem((int)resource->type()); // NOTE: must match enum
    dia->units->setValue(resource->units());
    dia->availableFrom->setDateTime(resource->availableFrom());
    dia->availableUntil->setDateTime(resource->availableUntil());
    dia->rateEdit->setText(KGlobal::locale()->formatMoney(resource->normalRate()));
    dia->overtimeEdit->setText(KGlobal::locale()->formatMoney(resource->overtimeRate()));

    int cal = 0;
    dia->calendarList->insertItem(i18n("None"));
    m_calendars.insert(0, 0);
    Q3PtrList<Calendar> list = project.calendars();
    Q3PtrListIterator<Calendar> cit = list;
    for(int i=1; cit.current(); ++cit, ++i) {
        dia->calendarList->insertItem(cit.current()->name(), i);
        m_calendars.insert(i, cit.current());
        if (cit.current() == resource->calendar())
            cal = i;
    }
    dia->calendarList->setCurrentItem(cal);

    connect(dia, SIGNAL(changed()), SLOT(enableButtonOk()));
    connect(dia, SIGNAL(calculate()), SLOT(slotCalculationNeeded()));
    connect(dia->calendarList, SIGNAL(activated(int)), SLOT(slotCalendarChanged(int)));

}


void ResourceDialog::enableButtonOk() {
    enableButtonOK(true);
}

void ResourceDialog::slotCalculationNeeded() {
    m_calculationNeeded = true;
}

void ResourceDialog::slotOk() {
    m_resource.setName(dia->nameEdit->text());
    m_resource.setInitials(dia->initialsEdit->text());
    m_resource.setEmail(dia->emailEdit->text());
    m_resource.setType((Resource::Type)(dia->type->currentItem()));
    m_resource.setUnits(dia->units->value());

    m_resource.setNormalRate(KGlobal::locale()->readMoney(dia->rateEdit->text()));
    m_resource.setOvertimeRate(KGlobal::locale()->readMoney(dia->overtimeEdit->text()));
    m_resource.setCalendar(m_calendars[dia->calendarList->currentItem()]);
    m_resource.setAvailableFrom(dia->availableFrom->dateTime());
    m_resource.setAvailableUntil(dia->availableUntil->dateTime());
    accept();
}

void ResourceDialog::slotCalendarChanged(int /*cal*/) {

}

KCommand *ResourceDialog::buildCommand(Part *part) {
    return buildCommand(m_original, m_resource, part);
}

// static
KCommand *ResourceDialog::buildCommand(Resource *original, Resource &resource, Part *part) {
    KMacroCommand *m=0;
    QString n = i18n("Modify Resource");
    if (resource.name() != original->name()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceNameCmd(part, original, resource.name()));
    }
    if (resource.initials() != original->initials()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceInitialsCmd(part, original, resource.initials()));
    }
    if (resource.email() != original->email()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceEmailCmd(part, original, resource.email()));
    }
    if (resource.type() != original->type()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceTypeCmd(part, original, resource.type()));
    }
    if (resource.units() != original->units()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceUnitsCmd(part, original, resource.units()));
    }
    if (resource.availableFrom() != original->availableFrom()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceAvailableFromCmd(part, original, resource.availableFrom()));
    }
    if (resource.availableUntil() != original->availableUntil()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceAvailableUntilCmd(part, original, resource.availableUntil()));
    }
    if (resource.normalRate() != original->normalRate()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceNormalRateCmd(part, original, resource.normalRate()));
    }
    if (resource.overtimeRate() != original->overtimeRate()) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceOvertimeRateCmd(part, original, resource.overtimeRate()));
    }
    if (resource.calendar(true) != original->calendar(true)) {
        if (!m) m = new KMacroCommand(n);
        m->addCommand(new ModifyResourceCalendarCmd(part, original, resource.calendar(true)));
    }
    return m;
}

}  //KPlato namespace

#include "kptresourcedialog.moc"
