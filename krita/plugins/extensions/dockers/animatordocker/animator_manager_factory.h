/*
 *
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ANIMATOR_MANAGER_FACTORY_H
#define ANIMATOR_MANAGER_FACTORY_H

#include <QObject>

#include "animator_manager.h"

#include <kis_image.h>
#include <kis_canvas2.h>

class AnimatorManagerFactory : public QObject
{
    Q_OBJECT

public:
    AnimatorManagerFactory();
    virtual ~AnimatorManagerFactory();
    
public:
    static AnimatorManagerFactory* instance();
    
public:
    AnimatorManager* getManager(KisImage* image, KisCanvas2* canvas);
    AnimatorManager* getManager(KisCanvas2* canvas);
    AnimatorManager* getManager(KisImage* image);
    
private:
    void init();
    
private:
    QMap<KisImage*,AnimatorManager*> m_instances;
};

#endif // ANIMATOR_MODEL_FACTORY_H
