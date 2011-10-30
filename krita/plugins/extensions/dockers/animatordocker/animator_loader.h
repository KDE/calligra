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

#ifndef ANIMATOR_LOADER_H
#define ANIMATOR_LOADER_H

#include <QObject>
#include "animator_manager.h"

class AnimatorLoader : public QObject
{
    Q_OBJECT
    
public:
    AnimatorLoader(AnimatorManager* manager);
    
public:
    virtual void loadAll();
    virtual void loadLayers(KisNodeSP rootNode);
    virtual void loadLayer(KisNodeSP node);
    
protected:
    virtual bool loadLegacy(int majorv);
    virtual bool convertAll();
    virtual bool convertLayers(KisNodeSP rootNode);
    virtual void convertLayer(KisNodeSP node);
    
protected:
    template <class CustomAnimatedLayer, class CustomFrameLayer> CustomAnimatedLayer* loadFramedLayer(KisNodeSP node);

private:
    AnimatorManager* m_manager;
};

#endif // ANIMATOR_LOADER_H
