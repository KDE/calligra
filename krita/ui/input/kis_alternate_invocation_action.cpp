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

#include "kis_alternate_invocation_action.h"

#include <KLocalizedString>

#include <KoColor.h>
#include <KoCanvasResourceManager.h>

#include <kis_paint_device.h>
#include <kis_canvas2.h>
#include <kis_view2.h>

#include "kis_input_manager.h"

class KisAlternateInvocationAction::Private
{
public:
    KoColor pick(KisPaintDeviceSP dev, const QPoint& pos);
};

KisAlternateInvocationAction::KisAlternateInvocationAction(KisInputManager *manager)
    : KisAbstractInputAction(manager), d(new Private)
{

}

KisAlternateInvocationAction::~KisAlternateInvocationAction()
{
    delete d;
}

void KisAlternateInvocationAction::begin(int /*shortcut*/)
{
    qDebug(Q_FUNC_INFO);
    KisPaintDeviceSP paintDevice = m_inputManager->canvas()->view()->activeDevice();
    m_inputManager->canvas()->resourceManager()->setResource(KoCanvasResourceManager::ForegroundColor, d->pick(paintDevice, m_inputManager->mousePosition().toPoint()));
}

void KisAlternateInvocationAction::end()
{

}

void KisAlternateInvocationAction::inputEvent(QEvent* event)
{
    Q_UNUSED(event);
}

QString KisAlternateInvocationAction::name() const
{
    return i18n("Alternate Invocation");
}

QHash< QString, int > KisAlternateInvocationAction::shortcuts() const
{
    QHash< QString, int> values;
    values.insert(i18nc("Invoke Tool shortcut for Alternate Invocation Action", "Invoke Tool"), 0);
    return values;
}

KoColor KisAlternateInvocationAction::Private::pick(KisPaintDeviceSP dev, const QPoint& pos)
{
    KoColor pickedColor;
    dev->pixel(pos.x(), pos.y(), &pickedColor);
    return pickedColor;
}