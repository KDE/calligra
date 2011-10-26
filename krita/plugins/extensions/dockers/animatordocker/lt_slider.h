/*
 *  Widget for use in light table docker
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


#ifndef LT_SLIDER_H
#define LT_SLIDER_H

#include <QWidget>
#include <QCheckBox>
#include <QSlider>
#include <QBoxLayout>

class LTSlider : public QWidget
{
    Q_OBJECT

public:
    LTSlider(QObject* parent = 0);
    virtual ~LTSlider();

public slots:
    void setNumbner(int n);
    void opacityEmitter(int value);
    void visibilityEmitter(bool value);
    
    void setOpacity(double n);
    void setVisibility(bool o);
    
signals:
    void opacityChanged(int num, int value);
    void visibilityChanged(int num, bool value);
    
private:
    int m_number;
    QSlider* m_slider;
    QCheckBox* m_check;
    QVBoxLayout* m_layout;
};

#endif // LT_SLIDER_H
