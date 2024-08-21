/*  This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPageNavigatorButton_p.h"

// KF5
#include <PageAppDebug.h>
// Qt
#include <QAction>
#include <QIcon>

KoPageNavigatorButton::KoPageNavigatorButton(const char *iconName, QWidget *parent)
    : QToolButton(parent)
    , m_action(nullptr)
{
    setIcon(QIcon::fromTheme(QLatin1String(iconName)));
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
}

void KoPageNavigatorButton::setAction(QAction *action)
{
    if (!action) {
        warnPageApp << "Attempt to set a null action";
        return;
    }
    if (m_action) {
        warnPageApp << "Attempt to set a second action";
        return;
    }

    m_action = action;

    connect(this, &QAbstractButton::clicked, this, &KoPageNavigatorButton::onClicked);
    connect(m_action, &QAction::changed, this, &KoPageNavigatorButton::onActionChanged);

    onActionChanged();
}

void KoPageNavigatorButton::onActionChanged()
{
    setEnabled(m_action->isEnabled());

    // always updating the tooltip is a workaround around that KPrView only updates
    // the action texts with the "slide" variants later, they are not already
    // set in KoPAView::initActions()
    setToolTip(m_action->toolTip());
}

void KoPageNavigatorButton::onClicked()
{
    m_action->activate(QAction::Trigger);
}
