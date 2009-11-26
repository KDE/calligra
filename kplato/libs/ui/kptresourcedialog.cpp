/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptresourcedialog.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"
#include "kptresourcemodel.h"

#include <QPushButton>
#include <QList>
#include <qstringlist.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <k3command.h>
#include <kdatetimewidget.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

namespace KPlato
{

ResourceDialogImpl::ResourceDialogImpl (QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(group, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(type, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(units, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
    connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(initialsEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(emailEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));

    connect(calendarList, SIGNAL(activated(int)), SLOT(slotChanged()));

    connect(rateEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(overtimeEdit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));

    connect(chooseBtn, SIGNAL(clicked()), SLOT(slotChooseResource()));

    connect(availableFrom, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotChanged()));
    connect(availableUntil, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotChanged()));
    connect(availableFrom, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotAvailableFromChanged(const QDateTime&)));
    connect(availableUntil, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotAvailableUntilChanged(const QDateTime&)));
    
    connect( useRequired, SIGNAL( stateChanged( int ) ), SLOT( slotUseRequiredChanged( int ) ) );
}


void ResourceDialogImpl::slotChanged() {
    emit changed();
}

void ResourceDialogImpl::setCurrentIndexes( const QModelIndexList &lst )
{
    m_currentIndexes.clear();
    foreach ( const QModelIndex &idx, lst ) {
        m_currentIndexes << QPersistentModelIndex( idx );
    }
    useRequired->setCheckState( m_currentIndexes.isEmpty() ? Qt::Unchecked : Qt::Checked );
    if ( useRequired->isChecked() ) {
        required->setCurrentIndexes( m_currentIndexes );
    }
    required->setEnabled( useRequired->isChecked() );
}

void ResourceDialogImpl::slotUseRequiredChanged( int state )
{
    required->setEnabled( state );
    if ( state ) {
        required->setCurrentIndexes( m_currentIndexes );
    } else {
        m_currentIndexes = required->currentIndexes();
        required->setCurrentIndexes( QList<QPersistentModelIndex>() );
    }
    slotChanged();
}

void ResourceDialogImpl::slotAvailableFromChanged(const QDateTime&) {
    if (availableUntil->dateTime() < availableFrom->dateTime()) {
        disconnect(availableUntil, SIGNAL(dateTimeChanged(const QDateTime&)), this,  SLOT(slotAvailableUntilChanged(const QDateTime&)));
        //kDebug()<<"From:"<<availableFrom->dateTime().toString()<<" until="<<availableUntil->dateTime().toString();
        availableUntil->setDateTime(availableFrom->dateTime());
        connect(availableUntil, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotAvailableUntilChanged(const QDateTime&)));
    }
}

void ResourceDialogImpl::slotAvailableUntilChanged(const QDateTime&) {
    if (availableFrom->dateTime() > availableUntil->dateTime()) {
        disconnect(availableFrom, SIGNAL(dateTimeChanged(const QDateTime&)), this,  SLOT(slotAvailableFromChanged(const QDateTime&)));
        //kDebug()<<"Until:"<<availableUntil->dateTime().toString()<<" from="<<availableFrom->dateTime().toString();
        availableFrom->setDateTime(availableUntil->dateTime());
        connect(availableFrom, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(slotAvailableFromChanged(const QDateTime&)));
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
        QStringList l = a.assembledName().split(' ');
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
    : KDialog(parent),
      m_project( project ),
      m_original(resource),
      m_resource(resource),
      m_calculationNeeded(false)
{
    setObjectName(name);
    
    setCaption( i18n("Resource Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    dia = new ResourceDialogImpl(this);
    setMainWidget(dia);
    KDialog::enableButtonOk(false);

    if ( resource->parentGroup() == 0 ) {
        //HACK to handle calls from ResourcesPanel
        dia->groupLabel->hide();
        dia->group->hide();
    } else {
        foreach ( ResourceGroup *g, project.resourceGroups() ) {
            m_groups.insert( g->name(), g );
        }
        dia->group->addItems( m_groups.keys() );
        dia->group->setCurrentIndex( m_groups.values().indexOf( resource->parentGroup() ) );
    }
    dia->nameEdit->setText(resource->name());
    dia->initialsEdit->setText(resource->initials());
    dia->emailEdit->setText(resource->email());
    dia->type->setCurrentIndex((int)resource->type()); // NOTE: must match enum
    dia->units->setValue(resource->units());
    dia->availableFrom->setDateTime(resource->availableFrom().dateTime());
    dia->availableUntil->setDateTime(resource->availableUntil().dateTime());
    dia->rateEdit->setText(project.locale()->formatMoney(resource->normalRate()));
    dia->overtimeEdit->setText(project.locale()->formatMoney(resource->overtimeRate()));

    int cal = 0;
    dia->calendarList->addItem(i18n("None"));
    m_calendars.insert(0, 0);
    QList<Calendar*> list = project.allCalendars();
    int i=1;
    foreach (Calendar *c, list) {
        dia->calendarList->insertItem(i, c->name());
        m_calendars.insert(i, c);
        if (c == resource->calendar(true)) {
            cal = i;
         }
        ++i;
    }
    dia->calendarList->setCurrentIndex(cal);

    ResourceItemSFModel *m = new ResourceItemSFModel( this );
    m->setProject( &project );
    dia->required->setModel( m );
    dia->required->view()->expandAll();

    QItemSelectionModel *sm = dia->required->view()->selectionModel();
    foreach ( Resource *r, resource->requiredResources() ) {
        sm->select( m->index( r ), QItemSelectionModel::Select | QItemSelectionModel::Rows );
    }
    dia->setCurrentIndexes( sm->selectedRows() );

    connect(dia, SIGNAL(changed()), SLOT(enableButtonOk()));
    connect(dia, SIGNAL(calculate()), SLOT(slotCalculationNeeded()));
    connect(dia->calendarList, SIGNAL(activated(int)), SLOT(slotCalendarChanged(int)));
    connect(dia->required, SIGNAL(changed()), SLOT(enableButtonOk()));
}


void ResourceDialog::enableButtonOk() {
		KDialog::enableButtonOk(true);
}

void ResourceDialog::slotCalculationNeeded() {
    m_calculationNeeded = true;
}

void ResourceDialog::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        slotOk();
    } else {
        KDialog::slotButtonClicked(button);
    }
}

void ResourceDialog::slotOk() {
    if ( ! m_groups.isEmpty() ) {
        //HACK to handle calls from ResourcesPanel
        m_resource.setParentGroup( m_groups.value( dia->group->currentText() ) );
    }
    m_resource.setName(dia->nameEdit->text());
    m_resource.setInitials(dia->initialsEdit->text());
    m_resource.setEmail(dia->emailEdit->text());
    m_resource.setType((Resource::Type)(dia->type->currentIndex()));
    m_resource.setUnits(dia->units->value());

    m_resource.setNormalRate(m_project.locale()->readMoney(dia->rateEdit->text()));
    m_resource.setOvertimeRate(m_project.locale()->readMoney(dia->overtimeEdit->text()));
    m_resource.setCalendar(m_calendars[dia->calendarList->currentIndex()]);
    m_resource.setAvailableFrom(dia->availableFrom->dateTime());
    m_resource.setAvailableUntil(dia->availableUntil->dateTime());
    ResourceItemSFModel *m = static_cast<ResourceItemSFModel*>( dia->required->model() );
    QList<Resource*> lst;
    foreach ( const QModelIndex &i, dia->required->currentIndexes() ) {
        Resource *r = m->resource( i );
        if ( r ) lst << r;
    }
    m_resource.setRequiredResources( lst );
    accept();
}

void ResourceDialog::slotCalendarChanged(int /*cal*/) {

}

MacroCommand *ResourceDialog::buildCommand() {
    return buildCommand(m_original, m_resource);
}

// static
MacroCommand *ResourceDialog::buildCommand(Resource *original, Resource &resource) {
    MacroCommand *m=0;
    QString n = i18n("Modify Resource");
    if (resource.parentGroup() != 0 && resource.parentGroup() != original->parentGroup()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new MoveResourceCmd(resource.parentGroup(), original));
    }
    if (resource.name() != original->name()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceNameCmd(original, resource.name()));
    }
    if (resource.initials() != original->initials()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceInitialsCmd(original, resource.initials()));
    }
    if (resource.email() != original->email()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceEmailCmd(original, resource.email()));
    }
    if (resource.type() != original->type()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceTypeCmd(original, resource.type()));
    }
    if (resource.units() != original->units()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceUnitsCmd(original, resource.units()));
    }
    if (resource.availableFrom() != original->availableFrom()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceAvailableFromCmd(original, resource.availableFrom().dateTime()));
    }
    if (resource.availableUntil() != original->availableUntil()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceAvailableUntilCmd(original, resource.availableUntil().dateTime()));
    }
    if (resource.normalRate() != original->normalRate()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceNormalRateCmd(original, resource.normalRate()));
    }
    if (resource.overtimeRate() != original->overtimeRate()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceOvertimeRateCmd(original, resource.overtimeRate()));
    }
    if (resource.calendar(true) != original->calendar(true)) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyResourceCalendarCmd(original, resource.calendar(true)));
    }
    if (resource.requiredResources() != original->requiredResources()) {
        if (!m) m = new MacroCommand(n);
        m->addCommand(new ModifyRequiredResourcesCmd(original, resource.requiredResources()));
    }
    return m;
}

}  //KPlato namespace

#include "kptresourcedialog.moc"
