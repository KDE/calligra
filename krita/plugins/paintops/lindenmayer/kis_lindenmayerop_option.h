/*
 *  Copyright (c) 2008,2010 Lukáš Tvrdý <lukast.dev@gmail.com>
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_LINDENMAYEROP_OPTION_H
#define KIS_LINDENMAYEROP_OPTION_H

#include <kis_paintop_option.h>
#include <krita_export.h>

const QString LINDENMAYER_RADIUS = "Lindenmayer/radius";
const QString LINDENMAYER_CODE = "Lindenmayer/code";

class KisLindenmayerOpOptionsWidget;

class KisLindenmayerOpOption : public KisPaintOpOption
{
public:
    KisLindenmayerOpOption();
    ~KisLindenmayerOpOption();

    void setRadius(int radius) const;
    int radius() const;
    QString code() const;
    
    void writeOptionSetting(KisPropertiesConfiguration* setting) const;
    void readOptionSetting(const KisPropertiesConfiguration* setting);


private:

    KisLindenmayerOpOptionsWidget * m_options;

};

class LindenmayerProperties{
public:
    int radius;
    QString code;
    
    void readOptionSetting(const KisPropertiesConfiguration* settings){
        radius = settings->getInt(LINDENMAYER_RADIUS);
        code = settings->getString(LINDENMAYER_CODE).replace("<br>", QString('\n'));
    }
};

#endif
