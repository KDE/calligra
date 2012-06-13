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

#ifndef KIS_ALTERNATE_INVOCATION_ACTION_H
#define KIS_ALTERNATE_INVOCATION_ACTION_H

#include <input/kis_abstract_input_action.h>


class KisAlternateInvocationAction : public KisAbstractInputAction
{
public:
    explicit KisAlternateInvocationAction(KisInputManager *manager);
    virtual ~KisAlternateInvocationAction();

    virtual void begin(int /*shortcut*/);
    virtual void end();
    virtual void inputEvent(QEvent* event);

    virtual QString name() const;
    virtual QString description() const;
    virtual QHash< QString, int > shortcuts() const;
};

#endif // KIS_ALTERNATE_INVOCATION_ACTION_H
