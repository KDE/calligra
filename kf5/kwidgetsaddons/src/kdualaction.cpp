/* This file is part of the KDE libraries
 *
 * Copyright (c) 2010 Aurélien Gâteau <agateau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#include "kdualaction.h"

#include <QIcon>

#include "kdualaction_p.h"

//---------------------------------------------------------------------
// KDualActionPrivate
//---------------------------------------------------------------------
void KDualActionPrivate::init(KDualAction *q_ptr)
{
    q = q_ptr;
    autoToggle = true;
    isActive = false;

    QObject::connect(q, SIGNAL(triggered()), q, SLOT(slotTriggered()));
}

void KDualActionPrivate::updateFromCurrentState()
{
    KGuiItem& currentItem = item(isActive);
    QAction* qq = static_cast<QAction*>(q);
    qq->setIcon(currentItem.icon());
    qq->setText(currentItem.text());
    qq->setToolTip(currentItem.toolTip());
}

void KDualActionPrivate::slotTriggered()
{
    if (!autoToggle) {
        return;
    }
    q->setActive(!isActive);
    q->activeChangedByUser(isActive);
}

//---------------------------------------------------------------------
// KDualAction
//---------------------------------------------------------------------
KDualAction::KDualAction(const QString &inactiveText, const QString &activeText, QObject *parent)
: QAction(parent)
, d(new KDualActionPrivate)
{
    d->init(this);
    d->item(false).setText(inactiveText);
    d->item(true).setText(activeText);
    d->updateFromCurrentState();
}

KDualAction::KDualAction(QObject *parent)
: QAction(parent)
, d(new KDualActionPrivate)
{
    d->init(this);
}

KDualAction::~KDualAction()
{
    delete d;
}

void KDualAction::setActiveGuiItem(const KGuiItem &item)   { d->setGuiItem(true, item); }
KGuiItem KDualAction::activeGuiItem() const                { return d->item(true); }
void KDualAction::setInactiveGuiItem(const KGuiItem &item) { d->setGuiItem(false, item); }
KGuiItem KDualAction::inactiveGuiItem() const              { return d->item(false); }

void KDualAction::setActiveIcon(const QIcon &icon)   { d->setIcon(true, icon); }
QIcon KDualAction::activeIcon() const                { return d->item(true).icon(); }
void KDualAction::setInactiveIcon(const QIcon &icon) { d->setIcon(false, icon); }
QIcon KDualAction::inactiveIcon() const              { return d->item(false).icon(); }

void KDualAction::setActiveText(const QString &text)   { d->setText(true, text); }
QString KDualAction::activeText() const                { return d->item(true).text(); }
void KDualAction::setInactiveText(const QString &text) { d->setText(false, text); }
QString KDualAction::inactiveText() const              { return d->item(false).text(); }

void KDualAction::setActiveToolTip(const QString &toolTip)   { d->setToolTip(true, toolTip); }
QString KDualAction::activeToolTip() const                   { return d->item(true).toolTip(); }
void KDualAction::setInactiveToolTip(const QString &toolTip) { d->setToolTip(false, toolTip); }
QString KDualAction::inactiveToolTip() const                 { return d->item(false).toolTip(); }

void KDualAction::setIconForStates(const QIcon &icon)
{
    setInactiveIcon(icon);
    setActiveIcon(icon);
}

void KDualAction::setAutoToggle(bool value)
{
    d->autoToggle = value;
}

bool KDualAction::autoToggle() const
{
    return d->autoToggle;
}

void KDualAction::setActive(bool active)
{
    if (active == d->isActive) {
        return;
    }
    d->isActive = active;
    d->updateFromCurrentState();
    activeChanged(active);
}

bool KDualAction::isActive() const
{
    return d->isActive;
}

#include "moc_kdualaction.cpp"
