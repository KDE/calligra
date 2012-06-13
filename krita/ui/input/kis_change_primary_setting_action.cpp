/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_change_primary_setting_action.h"

#include <KLocalizedString>

#include "kis_input_manager.h"
#include <KoToolProxy.h>

KisChangePrimarySettingAction::KisChangePrimarySettingAction(KisInputManager* manager)
    : KisAbstractInputAction(manager)
{

}

KisChangePrimarySettingAction::~KisChangePrimarySettingAction()
{

}

void KisChangePrimarySettingAction::begin(int shortcut)
{
    QMouseEvent *mevent = new QMouseEvent(QEvent::MouseButtonPress, m_inputManager->mousePosition().toPoint(), Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier);
    m_inputManager->toolProxy()->mousePressEvent(mevent, m_inputManager->mousePosition());
}

void KisChangePrimarySettingAction::end()
{
    QMouseEvent *mevent = new QMouseEvent(QEvent::MouseButtonRelease, m_inputManager->mousePosition().toPoint(), Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier);
    m_inputManager->toolProxy()->mouseReleaseEvent(mevent, m_inputManager->mousePosition());
}

void KisChangePrimarySettingAction::inputEvent(QEvent* event)
{
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
        m_inputManager->toolProxy()->mouseMoveEvent(mevent, mevent->posF());
    }
}

QString KisChangePrimarySettingAction::name() const
{
    return i18n("Change Tool Primary Setting");
}

QString KisChangePrimarySettingAction::description() const
{
    return i18n("Changes a tool's \"Primary Setting\", for example the brush size for the brush tool.");
}

QHash< QString, int > KisChangePrimarySettingAction::shortcuts() const
{
    QHash<QString, int> shortcuts;
    shortcuts.insert(i18n("Change Setting"), 0);
    return shortcuts;
}
