/*
 *  Interpolated layer: allows several ways of interpolating
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

#include "interpolated_animated_layer.h"
#include "animator_manager.h"
#include "animator_manager_factory.h"

InterpolatedAnimatedLayer::InterpolatedAnimatedLayer(const KisGroupLayer& source): FramedAnimatedLayer(source)
{
}

InterpolatedAnimatedLayer::InterpolatedAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): FramedAnimatedLayer(image, name, opacity)
{
}


void InterpolatedAnimatedLayer::updateFrame(int num)
{
    if (isKeyFrame(num))
        return;
    
    if (! enabled())
        return;
    
    setEnabled(false);
    
    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image().data());
    if (!manager->ready())
        return;
    
    int inxt = getNextKey(num);
    KisCloneLayer* next = 0;
    if (isKeyFrame(inxt))
        next = qobject_cast<KisCloneLayer*>( qobject_cast<SimpleFrameLayer*>(getKeyFrame(inxt))->getContent() );
    
    int ipre = getPreviousKey(num);
    KisNode* prev = 0;
    if (isKeyFrame(ipre))
        prev = qobject_cast<SimpleFrameLayer*>(getKeyFrame(ipre))->getContent();

    if (prev && next && next->inherits("KisCloneLayer"))
    {
        // interpolation here!
        double cur = num;
        double pre = ipre;
        double nxt = inxt;
        double p = (cur-pre) / (nxt-pre);
        
        SimpleFrameLayer* frame = qobject_cast<SimpleFrameLayer*>(frameAt(num));
        if (!frame)
        {
            manager->createFrame(this, "", false);           // this will create frame without content
            frame = qobject_cast<SimpleFrameLayer*>(frameAt(num));
        }
        manager->setFrameContent(frame, interpolate(prev, next, p));
    }
    
    setEnabled(true);
}

const QString& InterpolatedAnimatedLayer::getNameForFrame(int num, bool iskey) const
{
    QString *t = new QString(FramedAnimatedLayer::getNameForFrame(num, iskey));
    if (iskey)
        return *t;
    else
        return * ( new QString (*t+QString("_")) );
}

int InterpolatedAnimatedLayer::getFrameFromName(const QString& name, bool& iskey) const
{
    int result = FramedAnimatedLayer::getFrameFromName(name, iskey);
    return result;
}
