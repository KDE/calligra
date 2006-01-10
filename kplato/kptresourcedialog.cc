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
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qptrlist.h>
#include <qstringlist.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

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
    
}


void ResourceDialogImpl::slotChanged() {
    emit changed();
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
        for (int i=0; it != l.end(); ++it) {
            in += (*it)[0];
        }
        initialsEdit->setText(in);
    }
}

//////////////////  ResourceDialog  ////////////////////////

ResourceDialog::ResourceDialog(Project &project, Resource &resource, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Resource Settings"), Ok|Cancel, Ok, parent, name, true, true),
      m_resource(resource),
      m_calculationNeeded(false)
{
    dia = new ResourceDialogImpl(this);
    setMainWidget(dia);
    enableButtonOK(false);

    dia->nameEdit->setText(resource.name());
    dia->initialsEdit->setText(resource.initials());
    dia->emailEdit->setText(resource.email());
    dia->type->setCurrentItem((int)resource.type()); // NOTE: must match enum
    dia->units->setValue(resource.units());
    dia->availableFrom->setDateTime(resource.availableFrom());
    dia->availableUntil->setDateTime(resource.availableUntil());
    dia->rateEdit->setText(KGlobal::locale()->formatMoney(resource.normalRate()));
    dia->overtimeEdit->setText(KGlobal::locale()->formatMoney(resource.overtimeRate()));

    int cal = 0;
    dia->calendarList->insertItem(i18n("None"));
    m_calendars.insert(0, 0);      
    QPtrList<Calendar> list = project.calendars();
    QPtrListIterator<Calendar> cit = list;
    for(int i=1; cit.current(); ++cit, ++i) {
        dia->calendarList->insertItem(cit.current()->name(), i);
        m_calendars.insert(i, cit.current());
        if (cit.current() == resource.calendar())
            cal = i;
    }
    dia->calendarList->setCurrentItem(cal);

    connect(dia, SIGNAL(changed()), SLOT(enableButtonOk()));
    connect(dia, SIGNAL(calculate()), SLOT(slotCalculationNeeded()));
    connect(dia->calendarList, SIGNAL(activated(int)), SLOT(slotCalendarChanged(int)));
    connect(dia->availableFrom, SIGNAL(valueChanged(const QDateTime&)), SLOT(enableButtonOk()));
    connect(dia->availableUntil, SIGNAL(valueChanged(const QDateTime&)), SLOT(enableButtonOk()));
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

void ResourceDialog::slotCalendarChanged(int cal) {
    
}

}  //KPlato namespace

#include "kptresourcedialog.moc"
