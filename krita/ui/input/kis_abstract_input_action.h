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

#ifndef KIS_ABSTRACT_INPUT_ACTION_H
#define KIS_ABSTRACT_INPUT_ACTION_H

class QEvent;
class KisInputManager;
class KisAbstractInputAction
{
public:
    explicit KisAbstractInputAction(KisInputManager *manager) : m_inputManager(manager) { }
    virtual ~KisAbstractInputAction() { }

    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void inputEvent(QEvent* event) = 0;

protected:
    KisInputManager * const m_inputManager;
};

#endif // KIS_ABSTRACT_INPUT_ACTION_H
