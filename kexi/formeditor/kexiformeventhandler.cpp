/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexiformeventhandler.h"

#include <QWidget>

#include <kdebug.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kaction.h>

#include <dataviewcommon/kexitableviewdata.h>
#include <db/queryschema.h>
#include <KexiMainWindowIface.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>

KexiFormEventAction::ActionData::ActionData()
{
}

bool KexiFormEventAction::ActionData::isEmpty() const
{
    return string.isEmpty();
}

KexiPart::Info* KexiFormEventAction::ActionData::decodeString(
    QString& actionType, QString& actionArg, bool& ok) const
{
    const int idx = string.indexOf(':');
    ok = false;
    if (idx == -1)
        return 0;
    const QString _actionType = string.left(idx);
    const QString _actionArg = string.mid(idx + 1);
    if (_actionType.isEmpty() || _actionArg.isEmpty())
        return 0;
    KexiPart::Info *info = 0;
    if (_actionType != "kaction" && _actionType != "currentForm") {
        info = Kexi::partManager().infoForClass(QString("org.kexi-project.%1").arg(_actionType));
        if (!info)
            return 0;
    }
    actionType = _actionType;
    actionArg = _actionArg;
    ok = true;
    return info;
}

//-------------------------------------

class KexiFormEventAction::Private
{
public:
    Private(const QString& actionName_, const QString& objectName_, const QString actionOption_);

    ~Private();
    QString actionName, objectName, actionOption;
};

KexiFormEventAction::Private::Private(const QString& actionName_, const QString& objectName_, const QString actionOption_)
    :actionName(actionName_), objectName(objectName_), actionOption(actionOption_)
{

}

KexiFormEventAction::Private::~Private()
{

}

KexiFormEventAction::KexiFormEventAction(QObject* parent,
        const QString& actionName, const QString& objectName, const QString& actionOption)
        : KAction(parent)
        ,d(new Private(actionName, objectName, actionOption))
{
    connect(this, SIGNAL(triggered()), this, SLOT(trigger()));
}

KexiFormEventAction::~KexiFormEventAction()
{
    delete d;
}

void KexiFormEventAction::slotTrigger()
{
    kDebug() << d->actionName << d->objectName;
    KexiProject* project = KexiMainWindowIface::global()->project();
    if (!project)
        return;
    KexiPart::Part* part = Kexi::partManager().partForClass(
                               QString("org.kexi-project.%1").arg(d->actionName));
    if (!part)
        return;
    KexiPart::Item* item = project->item(part->info(), d->objectName);
    if (!item)
        return;
    bool actionCancelled = false;
    if (d->actionOption.isEmpty()) { // backward compatibility (good defaults)
        if (part->info()->isExecuteSupported())
            part->execute(item, parent());
        else
            KexiMainWindowIface::global()->openObject(item, Kexi::DataViewMode, actionCancelled);
    } else {
//! @todo react on failure...
        if (d->actionOption == "open")
            KexiMainWindowIface::global()->openObject(item, Kexi::DataViewMode, actionCancelled);
        else if (d->actionOption == "execute")
            part->execute(item, parent());
        else if (d->actionOption == "print") {
            if (part->info()->isPrintingSupported())
                KexiMainWindowIface::global()->printItem(item);
        }
#ifndef KEXI_NO_QUICK_PRINTING
        else if (d->actionOption == "printPreview") {
            if (part->info()->isPrintingSupported())
                KexiMainWindowIface::global()->printPreviewForItem(item);
        }
        else if (d->actionOption == "pageSetup") {
            if (part->info()->isPrintingSupported())
                KexiMainWindowIface::global()->showPageSetupForItem(item);
        }
#endif
        else if (d->actionOption == "exportToCSV"
                   || d->actionOption == "copyToClipboardAsCSV") {
            if (part->info()->isDataExportSupported())
                KexiMainWindowIface::global()->executeCustomActionForObject(item, d->actionOption);
        } else if (d->actionOption == "new")
            KexiMainWindowIface::global()->newObject(part->info(), actionCancelled);
        else if (d->actionOption == "design")
            KexiMainWindowIface::global()->openObject(item, Kexi::DesignViewMode, actionCancelled);
        else if (d->actionOption == "editText")
            KexiMainWindowIface::global()->openObject(item, Kexi::TextViewMode, actionCancelled);
        else if (d->actionOption == "close") {
            tristate res = KexiMainWindowIface::global()->closeObject(item);
            if (~res)
                actionCancelled = true;
        }
    }
}

//------------------------------------------

class KexiFormEventHandler::Private
{
public:
    Private();
    ~Private()
    {

    }

    QWidget *mainWidget;
};

KexiFormEventHandler::Private::Private() : mainWidget(0)
{

}

KexiFormEventHandler::KexiFormEventHandler()
    : d(new Private())
{

}

KexiFormEventHandler::~KexiFormEventHandler()
{
    delete d;
}

void KexiFormEventHandler::setMainWidgetForEventHandling(QWidget* mainWidget)
{
    d->mainWidget = mainWidget;
    if (!d->mainWidget)
        return;

    //find widgets whose will work as data items
//! @todo look for other widgets too
    QList<QWidget*> widgets(d->mainWidget->findChildren<QWidget*>());
    foreach(QWidget *widget, widgets) {
        if (!widget->inherits("QPushButton") ){
            continue;
        }
        bool ok;
        KexiFormEventAction::ActionData data;
        data.string = widget->property("onClickAction").toString();
        data.option = widget->property("onClickActionOption").toString();
        if (data.isEmpty())
            continue;

        QString actionType, actionArg;
        KexiPart::Info* partInfo = data.decodeString(actionType, actionArg, ok);
        if (!ok)
            continue;
kDebug() << "actionType:" << actionType << "actionArg:" << actionArg;
        if (actionType == "kaction" || actionType == "currentForm") {
            QAction *action = KexiMainWindowIface::global()->actionCollection()->action(
                                  actionArg);
            if (!action)
                continue;
            QObject::disconnect(widget, SIGNAL(clicked()), action, SLOT(trigger()));   //safety
            QObject::connect(widget, SIGNAL(clicked()), action, SLOT(trigger()));
        } else if (partInfo) { //'open or execute' action
            KexiFormEventAction* action = new KexiFormEventAction(widget, actionType, actionArg,
                    data.option);
            QObject::disconnect(widget, SIGNAL(clicked()), action, SLOT(slotTrigger()));
            QObject::connect(widget, SIGNAL(clicked()), action, SLOT(slotTrigger()));
        }
    }
}
