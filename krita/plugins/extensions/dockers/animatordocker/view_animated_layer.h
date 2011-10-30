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

#ifndef VIEW_ANIMATED_LAYER
#define VIEW_ANIMATED_LAYER

#include "animated_layer.h"

#include <QImage>

class ViewAnimatedLayer : public AnimatedLayer
{
    Q_OBJECT
    
public:
    ViewAnimatedLayer(const KisGroupLayer& source);
    ViewAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity);
    virtual ~ViewAnimatedLayer();
    
public:
    virtual QString aName() const;
    virtual void setAName(const QString& name);
    
    virtual bool displayable() const;
    virtual bool hasPreview() const;
    
    virtual FrameLayer* getCachedFrame(int num) const;
    
    virtual int dataStart() const;
    virtual int dataEnd() const;
    
    virtual void setStart(int st);
    virtual void setEnd(int end);
    
public:
    virtual void save();
    virtual void load();
    
public:
    virtual QImage getThumbnail(int fnum, int size);
    
private:
    QImage m_fullImage;
    int m_cached;
    
    int m_start;
    int m_end;
    KisNode* m_content;
};

#endif  // VIEW_ANIMATED_LAYER
