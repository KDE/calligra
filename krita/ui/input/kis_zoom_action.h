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

#ifndef KIS_ZOOM_ACTION_H
#define KIS_ZOOM_ACTION_H

#include "kis_abstract_input_action.h"

class KisZoomAction : public KisAbstractInputAction
{
public:
    enum Shortcuts {
        ZoomToggleShortcut,
        ZoomInShortcut,
        ZoomOutShortcut,
        ZoomResetShortcut,
        ZoomToPageShortcut
    };
    explicit KisZoomAction(KisInputManager* manager);
    virtual ~KisZoomAction();

    virtual void begin(int shortcut);
    virtual void end();
    virtual void inputEvent(QEvent* event);

private:
    class Private;
    Private * const d;
};

#endif // KIS_ZOOM_ACTION_H
