/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2008 Jarosław Staniek <staniek@kde.org>

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

#include "kexipropertyeditorview.h"
#include <KexiMainWindowIface.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>
#include <koproperty/Property.h>

#include <klocale.h>
#include <kiconloader.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

KexiObjectInfoLabel::KexiObjectInfoLabel(QWidget* parent)
        : QWidget(parent)
{
    QHBoxLayout *hlyr = new QHBoxLayout(this);
    hlyr->setContentsMargins(0, 0, 0, 0);
    hlyr->setSpacing(2);
    m_objectIconLabel = new QLabel(this);
    m_objectIconLabel->setMargin(2);
    setFixedHeight(IconSize(KIconLoader::Small) + 2 + 2);
    hlyr->addWidget(m_objectIconLabel);
    m_objectNameLabel = new QLabel(this);
//    m_objectNameLabel->setMargin(2);
    m_objectNameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    hlyr->addWidget(m_objectNameLabel);
}

KexiObjectInfoLabel::~KexiObjectInfoLabel()
{
}

void KexiObjectInfoLabel::setObjectClassIcon(const QString& name)
{
    m_classIcon = name;
    if (m_classIcon.isEmpty())
        m_objectIconLabel->setFixedWidth(0);
    else
        m_objectIconLabel->setFixedWidth(IconSize(KIconLoader::Small) + 2 + 2);
    m_objectIconLabel->setPixmap(SmallIcon(name));
}

void KexiObjectInfoLabel::setObjectClassName(const QString& name)
{
    m_className = name;
    updateName();
}

void KexiObjectInfoLabel::setObjectName(const QString& name)
{
    m_objectName = name;
    updateName();
}

void KexiObjectInfoLabel::updateName()
{
    QString txt(m_className);
    if (txt.isEmpty())
        txt = m_objectName;
    else if (!m_objectName.isEmpty())
        txt += QString(" \"%1\"").arg(m_objectName);
    m_objectNameLabel->setText(txt);
}

void KexiObjectInfoLabel::setBuddy(QWidget * buddy)
{
    m_objectNameLabel->setBuddy(buddy);
}

//------------------------------

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
        if (set->contains("this:classString"))
            className = (*set)["this:classString"].value().toString();
        if (set->contains("this:iconName"))
            iconName = (*set)["this:iconName"].value().toString();
        const bool useCaptionAsObjectName = set->contains("this:useCaptionAsObjectName")
                                            && (*set)["this:useCaptionAsObjectName"].value().toBool();
        if (set->contains(useCaptionAsObjectName ? "caption" : "name"))
            objectName = (*set)[useCaptionAsObjectName ? "caption" : "name"].value().toString();
        if (objectName.isEmpty() && useCaptionAsObjectName && set->contains("name")) // get name if there is no caption
            objectName = (*set)["name"].value().toString();
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

//------------------------------

//! @internal
class KexiPropertyEditorView::Private
{
public:
    Private() {
    }
    KoProperty::EditorView *editor;
};

//------------------------------

KexiPropertyEditorView::KexiPropertyEditorView(QWidget* parent)
        : KexiPropertyPaneViewBase(parent)
        , d(new Private())
{
    setObjectName("KexiPropertyEditorView");
    setWindowTitle(i18n("Properties"));
    //TODO: set a nice icon
//    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    d->editor = new KoProperty::EditorView(this); //, true /*AutoSync*/, "propeditor");
    layout()->addWidget(d->editor);
    setFocusProxy(d->editor);
    infoLabel()->setBuddy(d->editor);
    setFocusPolicy(Qt::WheelFocus);

    connect(d->editor, SIGNAL(propertySetChanged(KoProperty::Set*)),
            this, SLOT(slotPropertySetChanged(KoProperty::Set*)));

    slotPropertySetChanged(0);
}

KexiPropertyEditorView::~KexiPropertyEditorView()
{
    delete d;
}

QSize KexiPropertyEditorView::sizeHint() const
{
    return QSize(200, 200);//m_editor->sizeHint();
}

QSize KexiPropertyEditorView::minimumSizeHint() const
{
    return QSize(200, 200);//m_editor->sizeHint();
}

/*void KexiPropertyEditorView::setGeometry ( const QRect &r )
{
  QWidget::setGeometry(r);
}

void KexiPropertyEditorView::resize (  int w, int h  )
{
  QWidget::resize( w, h );
}*/

KoProperty::EditorView *KexiPropertyEditorView::editor() const
{
    return d->editor;
}

void KexiPropertyEditorView::slotPropertySetChanged(KoProperty::Set* set)
{
    //update information about selected object
    updateInfoLabelForPropertySet(set);
    d->editor->setEnabled(set);
}

#include "kexipropertyeditorview.moc"
