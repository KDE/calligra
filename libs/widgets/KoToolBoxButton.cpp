/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxButton_p.h"

#include <KoIcon.h>
#include <KoToolManager.h>

#include <QApplication>
#include <QIcon>
#include <QKeySequence>
#include <QPalette>

#include <KLocalizedString>

KoToolBoxButton::KoToolBoxButton(KoToolAction *toolAction, QWidget *parent)
    : QToolButton(parent)
    , m_toolAction(toolAction)
{
    setObjectName(m_toolAction->id());
    // ensure same L&F
    setCheckable(true);
    setAutoRaise(true);
    setIcon(QIcon::fromTheme(m_toolAction->iconName()));

    setDataFromToolAction();

    connect(this, &QAbstractButton::clicked, m_toolAction, &KoToolAction::trigger);
    connect(m_toolAction, &KoToolAction::changed, this, &KoToolBoxButton::setDataFromToolAction);
}

void KoToolBoxButton::setHighlightColor()
{
    QPalette p = qApp->palette();
    if (isChecked()) {
        QPalette palette_highlight(p);
        const QColor &c = p.color(QPalette::Highlight);
        palette_highlight.setColor(QPalette::Button, c);
        setPalette(palette_highlight);
    } else {
        setPalette(p);
    }
}

void KoToolBoxButton::setDataFromToolAction()
{
    const QString plainToolTip = m_toolAction->toolTip();
    const QKeySequence shortcut = m_toolAction->shortcut();
    const QString toolTip =
        shortcut.isEmpty() ? i18nc("@info:tooltip", "%1", plainToolTip) : i18nc("@info:tooltip %2 is shortcut", "%1 (%2)", plainToolTip, shortcut.toString());

    setToolTip(toolTip);
}
