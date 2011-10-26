/*
 *  Light table model
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


#ifndef ANIMATOR_LT_H
#define ANIMATOR_LT_H

#include <QList>
#include <QObject>

class AnimatorLT : public QObject
{
    Q_OBJECT
    
public:
    AnimatorLT();
    virtual ~AnimatorLT();
    
public:
    double getOpacity(int rel_frame);
    bool getVisibility(int rel_frame);
    int getNear();
    
signals:
    void nearChanged(int near);
    void opacityChanged();
    void visibilityChanged();
    
public slots:
    void setOpacity(int rel_frame, double op);
    void setOpacity(int rel_frame, int op);
    void setVisibility(int rel_frame, bool ch);
    
    void setNear(int near);
    
protected:
    void update();
    
private:
    QList<double> m_opacity;
    QList<bool> m_vis;
    
    bool m_manually;
    quint8 m_near;
};

#endif // ANIMATOR_LT_H
