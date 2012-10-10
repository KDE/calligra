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

#include "SketchInputContext.h"
#include <QProcess>

SketchInputContext::SketchInputContext(QObject* parent): QInputContext(parent)
{
    m_keyboardApplication = new QProcess(this);
}

SketchInputContext::~SketchInputContext()
{

}

bool SketchInputContext::isComposing() const
{
    return true;
}

void SketchInputContext::reset()
{

}

QString SketchInputContext::language()
{
    return QString("en_US");
}

QString SketchInputContext::identifierName()
{
    return QString("SketchInputContext");
}

bool SketchInputContext::filterEvent(const QEvent* event)
{
    if(event->type() == QEvent::RequestSoftwareInputPanel) {
        m_keyboardApplication->start("C:\Program Files\Common Files\Microsoft Shared\ink\TabTip.exe");
        return true;
    } //else if(event->type() == QEvent::
    return false;
}
