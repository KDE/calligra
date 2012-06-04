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

#ifndef KIS_INPUTMANAGER_H
#define KIS_INPUTMANAGER_H

#include <QtCore/QObject>

class KoToolProxy;
class KisCanvas2;
class KisInputAction;
class KisInputManager : public QObject
{
    Q_OBJECT

public:
    explicit KisInputManager(KisCanvas2* canvas, KoToolProxy* proxy);
    ~KisInputManager();

    bool eventFilter(QObject* object, QEvent* event );

public Q_SLOTS:
    void actionEnded();

private:
    class Private;
    Private * const d;
};

#endif // KIS_INPUTMANAGER_H
