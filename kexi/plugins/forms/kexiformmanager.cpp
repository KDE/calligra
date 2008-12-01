/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexiformmanager.h"
#include "widgets/kexidbform.h"
#include "widgets/kexidbautofield.h"
#include "kexiformscrollview.h"
#include "kexiformview.h"
#include "kexidatasourcepage.h"

#include <kaction.h>
#include <kactioncollection.h>

#include <formeditor/formmanager.h>
#include <formeditor/widgetpropertyset.h>
#include <formeditor/form.h>
#include <formeditor/widgetlibrary.h>
#include <formeditor/commands.h>
#include <formeditor/objecttree.h>

#include <koproperty/Set.h>
#include <koproperty/Property.h>
#include <widget/kexicustompropertyfactory.h>
#include <core/KexiMainWindowIface.h>

KexiFormManager::KexiFormManager(KexiPart::Part *parent, const char* name)
        : KFormDesigner::FormManager(parent,
                                     KFormDesigner::FormManager::HideEventsInPopupMenu |
                                     KFormDesigner::FormManager::SkipFileActions |
                                     KFormDesigner::FormManager::HideSignalSlotConnections
                                     , name)
        , m_part(parent)
{
    m_emitSelectionSignalsUpdatesPropertySet = true;
    KexiCustomPropertyFactory::init();
}

KexiFormManager::~KexiFormManager()
{
}

QAction* KexiFormManager::action(const char* name)
{
    KActionCollection *col = m_part->actionCollectionForMode(Kexi::DesignViewMode);
    if (!col)
        return 0;
    QString n(translateName(name));
    QAction *a = col->action(n);
    if (a)
        return a;
    KexiDBForm *dbform;
    if (!activeForm() || !activeForm()->designMode()
            || !(dbform = dynamic_cast<KexiDBForm*>(activeForm()->formWidget())))
        return 0;
    KexiFormScrollView *scrollViewWidget
    = dynamic_cast<KexiFormScrollView*>(dbform->dataAwareObject());
    if (!scrollViewWidget)
        return 0;
    KexiFormView* formViewWidget = dynamic_cast<KexiFormView*>(scrollViewWidget->parent());
    if (!formViewWidget)
        return 0;
    return KexiMainWindowIface::global()->actionCollection()->action(n);
}

KexiFormView* KexiFormManager::activeFormViewWidget() const
{
    KexiDBForm *dbform;
    if (!activeForm() || !activeForm()->designMode()
            || !(dbform = dynamic_cast<KexiDBForm*>(activeForm()->formWidget())))
        return 0;
    KexiFormScrollView *scrollViewWidget = dynamic_cast<KexiFormScrollView*>(dbform->dataAwareObject());
    if (!scrollViewWidget)
        return 0;
    return dynamic_cast<KexiFormView*>(scrollViewWidget->parent());
}

void KexiFormManager::enableAction(const char* name, bool enable)
{
    KexiFormView* formViewWidget = activeFormViewWidget();
    if (!formViewWidget)
        return;
// if (QString(name)=="layout_menu")
//  kDebug() << "!!!!!!!!!!! " << enable;
    formViewWidget->setAvailable(translateName(name).toLatin1(), enable);
}

void KexiFormManager::setFormDataSource(const QString& partClass, const QString& name)
{
    if (!activeForm())
        return;
    KexiDBForm* formWidget = dynamic_cast<KexiDBForm*>(activeForm()->widget());
    if (!formWidget)
        return;

// setPropertyValueInDesignMode(formWidget, "dataSource", name);

    QString oldDataSourcePartClass(formWidget->dataSourcePartClass());
    QString oldDataSource(formWidget->dataSource());
    if (partClass != oldDataSourcePartClass || name != oldDataSource) {
        QMap<Q3CString, QVariant> propValues;
        propValues.insert("dataSource", name);
        propValues.insert("dataSourcePartClass", partClass);
        KFormDesigner::CommandGroup *group
        = new KFormDesigner::CommandGroup(
            i18n("Set Form's Data Source to \"%1\"", name), propertySet());
        propertySet()->createPropertyCommandsInDesignMode(
            formWidget, propValues, group, true /*addToActiveForm*/);
    }
}

void KexiFormManager::setDataSourceFieldOrExpression(
    const QString& string, const QString& caption, KexiDB::Field::Type type)
{
    if (!activeForm())
        return;
// KexiFormDataItemInterface* dataWidget = dynamic_cast<KexiFormDataItemInterface*>(activeForm()->selectedWidget());
// if (!dataWidget)
//  return;

    KFormDesigner::WidgetPropertySet *set = propertySet();
    if (!set || !set->contains("dataSource"))
        return;

    (*set)["dataSource"].setValue(string);

    if (set->contains("autoCaption") && (*set)["autoCaption"].value().toBool()) {
        if (set->contains("fieldCaptionInternal"))
            (*set)["fieldCaptionInternal"].setValue(caption);
    }
    if (//type!=KexiDB::Field::InvalidType &&
        set->contains("widgetType") && (*set)["widgetType"].value().toString() == "Auto") {
        if (set->contains("fieldTypeInternal"))
            (*set)["fieldTypeInternal"].setValue(type);
    }

    /* QString oldDataSource( dataWidget->dataSource() );
      if (string!=oldDataSource) {
        dataWidget->setDataSource(string);
        emit dirty(activeForm(), true);

        buffer
      }*/
}

void KexiFormManager::insertAutoFields(const QString& sourcePartClass, const QString& sourceName,
                                       const QStringList& fields)
{
    KexiFormView* formViewWidget = activeFormViewWidget();
    if (!formViewWidget || !formViewWidget->form() || !formViewWidget->form()->activeContainer())
        return;
    formViewWidget->insertAutoFields(sourcePartClass, sourceName, fields,
                                     formViewWidget->form()->activeContainer());
}

void KexiFormManager::slotHistoryCommandExecuted()
{
    const KFormDesigner::CommandGroup *group = dynamic_cast<const KFormDesigner::CommandGroup*>(sender());
    if (group) {
        if (group->commands().count() == 2) {
            KexiDBForm* formWidget = dynamic_cast<KexiDBForm*>(activeForm()->widget());
            if (!formWidget)
                return;
            QList<K3Command*>::const_iterator it(group->commands().constBegin());
            const KFormDesigner::PropertyCommand* pc1
            = dynamic_cast<const KFormDesigner::PropertyCommand*>(*it);
            ++it;
            const KFormDesigner::PropertyCommand* pc2
            = dynamic_cast<const KFormDesigner::PropertyCommand*>(*it);
            if (pc1 && pc2 && pc1->property() == "dataSource" && pc2->property() == "dataSourcePartClass") {
                const QMap<Q3CString, QVariant>::const_iterator it1(pc1->oldValues().constBegin());
                const QMap<Q3CString, QVariant>::const_iterator it2(pc2->oldValues().constBegin());
                if (it1.key() == formWidget->objectName() && it2.key() == formWidget->objectName())
                    static_cast<KexiFormPart*>(m_part)->dataSourcePage()->setDataSource(
                        formWidget->dataSourcePartClass(), formWidget->dataSource());
            }
        }
    }
}

/*
bool KexiFormManager::loadFormFromDomInternal(Form *form, QWidget *container, QDomDocument &inBuf)
{
  QMap<QCString,QString> customProperties;
  FormIO::loadFormFromDom(myform, container, domDoc, &customProperties);
}

bool KexiFormManager::saveFormToStringInternal(Form *form, QString &dest, int indent)
{
  QMap<QCString,QString> customProperties;
  return KFormDesigner::FormIO::saveFormToString(form, dest, indent, &customProperties);
}

*/

#include "kexiformmanager.moc"
