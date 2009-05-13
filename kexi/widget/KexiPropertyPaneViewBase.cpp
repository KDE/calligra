/* This file is part of the KDE project
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiPropertyPaneViewBase.h"
#include "KexiObjectInfoLabel.h"
#include <KexiMainWindowIface.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>
#include <koproperty/Property.h>

#include <klocale.h>
#include <kiconloader.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

//! @internal
class KexiPropertyPaneViewBase::Private
{
public:
    Private() {
    }
    KexiObjectInfoLabel *infoLabel;
};

KexiPropertyPaneViewBase::KexiPropertyPaneViewBase(QWidget* parent)
        : QWidget(parent)
        , d(new Private())
{
    //TODO: set a nice icon
//    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(2, 2, 2, 2);
    lyr->setSpacing(2);

    //add object class info
    d->infoLabel = new KexiObjectInfoLabel(this);
    d->infoLabel->setObjectName("KexiObjectInfoLabel");
    lyr->addWidget(d->infoLabel);
}

KexiPropertyPaneViewBase::~KexiPropertyPaneViewBase()
{
    delete d;
}

KexiObjectInfoLabel *KexiPropertyPaneViewBase::infoLabel() const
{
    return d->infoLabel;
}

void KexiPropertyPaneViewBase::updateInfoLabelForPropertySet(
        KoProperty::Set* set, const QString& textToDisplayForNullSet)
{
    QString className, iconName, objectName;
    if (set) {
        className = set->propertyValue("this:classString").toString();
        iconName = set->propertyValue("this:iconName").toString();
        const bool useCaptionAsObjectName
            = set->propertyValue("this:useCaptionAsObjectName", false).toBool();
        objectName = set->propertyValue(
            useCaptionAsObjectName ? "caption" : "objectName").toString();
        if (objectName.isEmpty() && useCaptionAsObjectName) {
            // get name if there is no caption
            objectName = set->propertyValue("objectName").toString();
        }
    }
    if (!set || objectName.isEmpty()) {
        objectName = textToDisplayForNullSet;
        className.clear();
        iconName.clear();
    }

    if (className.isEmpty() && objectName.isEmpty())
        d->infoLabel->hide();
    else
        d->infoLabel->show();

    if (d->infoLabel->objectClassName() == className
            && d->infoLabel->objectClassIcon() == iconName
            && d->infoLabel->objectName() == objectName)
        return;

    d->infoLabel->setObjectClassIcon(iconName);
    d->infoLabel->setObjectClassName(className);
    d->infoLabel->setObjectName(objectName);
}

#include "KexiPropertyPaneViewBase.moc"
