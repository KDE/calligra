/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
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
#ifndef KOCONTROLLERBASE_H
#define KOCONTROLLERBASE_H

#include "komvc_export.h"
#include <QObject>

class KoMainWindowBase;

/**
 * @brief The KoControllerBase class is a base class for classes that handle input that
 * modifies a document. It provides the action list and input manager.
 */
class KOMVC_EXPORT KoControllerBase : public QObject
{
    Q_OBJECT
public:
    explicit KoControllerBase(QObject *parent = 0);

    virtual void createToolbars(KoMainWindowBase *mainWindow) = 0;
    virtual void createMenus(KoMainWindowBase *mainWindow) = 0;
    virtual void createDockers(KoMainWindowBase *mainWindow) = 0;
    virtual void createStatusbar(KoMainWindowBase *mainWindow) = 0;

};

#endif // KOCONTROLLERBASE_H
