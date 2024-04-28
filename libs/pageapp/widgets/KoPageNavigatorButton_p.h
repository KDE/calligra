/*  This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAGENAVIGATORBUTTON_H
#define KOPAGENAVIGATORBUTTON_H

// Qt
#include <QToolButton>

class QAction;

class KoPageNavigatorButton : public QToolButton
{
    Q_OBJECT

public:
    KoPageNavigatorButton(const char *iconName, QWidget *parent);

    void setAction(QAction *action);
    QAction *action() const
    {
        return m_action;
    }

private Q_SLOTS:
    void onActionChanged();
    void onClicked();

private:
    QAction *m_action;
};

#endif
