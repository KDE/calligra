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

#ifndef ANIMATOR_LT_UPDATER_H
#define ANIMATOR_LT_UPDATER_H

#include "animator_updater.h"
#include "animator_lt.h"


class AnimatorLTUpdater : public AnimatorUpdater
{
    Q_OBJECT
    
public:
    enum LTUpdaterMode {
        Disabled,
        Normal,
        KeyFramed
    };
    
public:
    AnimatorLTUpdater(AnimatorManager* manager);
    virtual ~AnimatorLTUpdater();
    
public:
    virtual void updateLayer(AnimatedLayer* layer, int oldFrame, int newFrame);
    
public:
    virtual void setMode(LTUpdaterMode mode);
    virtual LTUpdaterMode mode() const;
    
public:
    virtual AnimatorLT *getLT();
    
public:
    virtual void setFilter(KisAdjustmentLayerSP filter);
    
protected slots:
    virtual void updateRelFrame(int relFrame);
    
protected:
    virtual void setupFilter(FrameLayer *frame, int rel);
    
private:
    AnimatorLT* m_LT;
    
    LTUpdaterMode m_mode;
    
private:
    KisAdjustmentLayer *m_filter;
};

#endif // ANIMATOR_LT_UPDATER_H
