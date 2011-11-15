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

#ifndef ANIMATOR_LT_VIEW_H
#define ANIMATOR_LT_VIEW_H

#include <QWidget>
#include <QSlider>
#include <QBoxLayout>
#include <QLayoutItem>
#include <QSpinBox>

#include "lt_slider.h"
#include "animator_filtered_lt.h"

class AnimatorLTView : public QWidget
{
    Q_OBJECT

public:
    AnimatorLTView(QWidget* parent = 0);
    virtual ~AnimatorLTView();
    
public:
    virtual void setLT(AnimatorLT* lt);
    
protected slots:
    virtual void slidersUpdate();
    virtual void setupUI();
    virtual void setLeftFilter();
    virtual void setRightFilter();
    
protected:
    virtual void setFilter(int relFrame);
    
private:
    AnimatorFilteredLT* m_lt;
    
private:
    QVBoxLayout* m_layout;
    QHBoxLayout* m_slidersLayout;
    QList<LTSlider*> m_sliders;
    QSpinBox* m_nearSpinbox;
    
    // Spacers
    QSpacerItem* m_spL;
    QSpacerItem* m_spR;
};

#endif // ANIMATOR_LT_VIEW_H
