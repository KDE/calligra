/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include "widgetfactory.h"

#include "WidgetInfo.h"
#include "libactionwidget.h"

#include <KoIcon.h>

#include <kdebug.h>
#include <KActionCollection>

//unused #include "formmanager.h"

using namespace KFormDesigner;

LibActionWidget::LibActionWidget(ActionGroup *group, WidgetInfo *w)
        : KToggleAction(KIcon(w->iconName()), w->name(), group)
{
    setObjectName(QLatin1String("library_widget_") + w->className());
    group->addAction(this);
// kDebug() << QString("library_widget_" + w->className()).toLatin1();
    m_className = w->className();
//kde4 not needed setExclusiveGroup("LibActionWidgets");
    setToolTip(w->name());
    setWhatsThis(w->description());
// connect(this, SIGNAL(activated()), this, SLOT(slotWidget()));
}

LibActionWidget::~LibActionWidget()
{
}

void
LibActionWidget::slotToggled(bool checked)
{
    KToggleAction::slotToggled(checked);
    if (checked)
        emit toggled(m_className);
}

#include "libactionwidget.moc"
