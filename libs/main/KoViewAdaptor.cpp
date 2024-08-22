/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   $Id: KoViewAdaptor.cc 529520 2006-04-13 16:41:44Z mpfeiffer $

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoViewAdaptor.h"

#include "KoView.h"

#include <KActionCollection>
#include <QAction>
#include <QList>

KoViewAdaptor::KoViewAdaptor(KoView *view)
    : QDBusAbstractAdaptor(view)
{
    setAutoRelaySignals(true);
    m_pView = view;
}

KoViewAdaptor::~KoViewAdaptor() = default;

QStringList KoViewAdaptor::actions()
{
    QStringList tmp_actions;
    QList<QAction *> lst = m_pView->actionCollection()->actions();
    foreach (QAction *it, lst) {
        if (it->isEnabled())
            tmp_actions.append(it->objectName());
    }
    return tmp_actions;
}
