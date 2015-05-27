/*
 *  Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KIS_STACKEDPAINTOP_SETTINGS_WIDGET_H_
#define KIS_STACKEDPAINTOP_SETTINGS_WIDGET_H_

#include <kis_paintop_settings_widget.h>

#include "ui_wdgstackedoptions.h"

class StackedOpOption;

class StackedPaintOpSettingsWidget : public KisPaintOpSettingsWidget
{
    Q_OBJECT

public:
    StackedPaintOpSettingsWidget(QWidget* parent = 0);
    virtual ~StackedPaintOpSettingsWidget();

    KisPropertiesConfiguration* configuration() const;

    ///Reimplemented
    void changePaintOpSize(qreal x, qreal y);
    virtual QSizeF paintOpSize() const;
private:

    StackedOpOption *m_stackedOption;

};

#endif
