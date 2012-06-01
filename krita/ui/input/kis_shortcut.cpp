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

#include "kis_shortcut.h"

#include "kis_abstract_input_action.h"

class KisShortcut::Private
{
public:
    QList<Qt::Key> keys;
    QList<Qt::Key> keyState;
    QList<Qt::MouseButton> buttons;
    QList<Qt::MouseButton> buttonState;

    KisAbstractInputAction *action;
};

KisShortcut::KisShortcut() : d(new Private)
{

}

KisShortcut::~KisShortcut()
{

}

int KisShortcut::priority() const
{
    return d->keys.count() + d->buttons.count();
}

KisAbstractInputAction* KisShortcut::action() const
{
    return d->action;
}

void KisShortcut::setAction(KisAbstractInputAction* action)
{
    d->action = action;
}

void KisShortcut::setButtons(const QList<Qt::MouseButton> &buttons)
{
    d->buttons = buttons;
    d->buttonState.clear();
}

void KisShortcut::setKeys(const QList< Qt::Key >& keys)
{
    d->keys = keys;
    d->keyState.clear();
}

void KisShortcut::buttonPress(Qt::MouseButton button)
{
    if( d->buttons.contains( button ) && !d->buttonState.contains(button) ) {
        d->buttonState.append( button );
    }
}

void KisShortcut::buttonRelease(Qt::MouseButton button)
{
    if( d->buttonState.contains( button ) ) {
        d->buttonState.removeOne(button);
    }
}

void KisShortcut::keyPress(Qt::Key key)
{
    if( d->keys.contains(key) && !d->keyState.contains(key) ) {
        d->keyState.append(key);
    }
}

void KisShortcut::keyRelease(Qt::Key key)
{
    if( d->keyState.contains(key) ) {
        d->keyState.removeOne(key);
    }
}

KisShortcut::MatchLevel KisShortcut::matchLevel()
{
    if( d->keys.count() == d->keyState.count() && d->buttons.count() == d->buttonState.count() ) {
        return CompleteMatch;
    } else if( d->keyState.count() > 0 || d->buttonState.count() > 0 ) {
        return PotentialMatch;
    }

    return NoMatch;
}
