/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef COLORMATRIXEFFECTCONFIGWIDGET_H
#define COLORMATRIXEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class ColorMatrixEffect;
class KDoubleNumInput;
class KComboBox;

class ColorMatrixEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    ColorMatrixEffectConfigWidget(QWidget *parent = 0);
    
    /// reimplemented from KoFilterEffectConfigWidgetBase
    virtual bool editFilterEffect(KoFilterEffect * filterEffect);
    
private slots:
    void matrixChanged(double);
    void saturateChanged(double saturate);
    void hueRotateChanged(double angle);
    
private:
    KComboBox * m_type;
    ColorMatrixEffect * m_effect;
    KDoubleNumInput * m_matrix[20];
    KDoubleNumInput * m_saturate;
    KDoubleNumInput * m_hueRotate;
};

#endif // COLORMATRIXEFFECTCONFIGWIDGET_H
