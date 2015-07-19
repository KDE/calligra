/* This file is part of the KDE project
 *
 * Copyright (C) 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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


#include "kis_tangent_tilt_option.h"
#include <cmath>

#include "ui_wdgtangenttiltoption.h"

class KisTangentTiltOptionWidget: public QWidget, public Ui::WdgTangentTiltOptions
{
public:
    KisTangentTiltOptionWidget(QWidget *parent = 0)
        : QWidget(parent) {
        setupUi(this);
    }
};

KisTangentTiltOption::KisTangentTiltOption()
: KisPaintOpOption(KisPaintOpOption::GENERAL, false),
            m_canvasAngle(0.0),
            m_canvasAxisXMirrored(false),
            m_canvasAxisYMirrored(false)
{
    m_checkable = false;
    m_options = new KisTangentTiltOptionWidget();
    //Setup tangent tilt.
    m_options->comboRed->setCurrentIndex(0);
    m_options->comboGreen->setCurrentIndex(2);
    m_options->comboBlue->setCurrentIndex(4);
    m_options->spinElevationMin->setValue(0);
    m_options->spinElevationMax->setValue(360);
    //m_options->
    
    connect(m_options->comboRed, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));
    connect(m_options->comboGreen, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));
    connect(m_options->comboBlue, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));
    
    connect(m_options->optionTilt, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    connect(m_options->optionDirection, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    connect(m_options->optionRotation, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    
    connect(m_options->spinElevationMin, SIGNAL(valueChanged(int)), SLOT(emitSettingChanged()));
    connect(m_options->spinElevationMax, SIGNAL(valueChanged(int)), SLOT(emitSettingChanged()));
    
    //hiding these, they don't work.
    m_options->spinDirectionMin->setVisible(false);
    m_options->spinDirectionMax->setVisible(false);
    m_options->L_direction->setVisible(false);
    //connect(m_options->spinDirectionMin, SIGNAL(valueChanged(int)), SLOT(emitSettingChanged()));
    //connect(m_options->spinDirectionMax, SIGNAL(valueChanged(int)), SLOT(emitSettingChanged()));
    //connect tangent tilt
    setConfigurationPage(m_options);

}
KisTangentTiltOption::~KisTangentTiltOption()
{
    delete m_options;
}

//options
int KisTangentTiltOption::redChannel() const
{
    return m_options->comboRed->currentIndex(); 
}
int KisTangentTiltOption::greenChannel() const
{
    return m_options->comboGreen->currentIndex(); 
}
int KisTangentTiltOption::blueChannel() const
{
    return m_options->comboBlue->currentIndex(); 
}

int KisTangentTiltOption::directionType() const
{
    int type=0;
    
    if (m_options->optionTilt->isChecked()==true) {
        type=0;
    }
    else if (m_options->optionDirection->isChecked()==true) {
        type=1;
    }
    else if (m_options->optionRotation->isChecked()==true) {
        type=2;
    }
    else {
        qWarning()<<"There's something odd with the radio buttons. We'll use Tilt";
    }
    
    return type;
        
}

int KisTangentTiltOption::elevationMin() const
{
    return m_options->spinElevationMin->value(); 
}

int KisTangentTiltOption::elevationMax() const
{
    return m_options->spinElevationMax->value(); 
}

int KisTangentTiltOption::directionMin() const
{
    return m_options->spinDirectionMin->value(); 
}

int KisTangentTiltOption::directionMax() const
{
    return m_options->spinDirectionMax->value(); 
}

void KisTangentTiltOption::swizzleAssign(qreal const horizontal, qreal const vertical, qreal const depth, quint8 *component, int index, qreal maxvalue)
{
    switch(index) {
    case 0: *component = horizontal; break;
    case 1: *component = maxvalue-horizontal; break;
    case 2: *component = vertical; break;
    case 3: *component = maxvalue-vertical; break;
    case 4: *component = depth; break;
    case 5: *component = maxvalue-depth; break;
    }
}

void KisTangentTiltOption::apply(const KisPaintInformation& info,quint8 *r,quint8 *g,quint8 *b)
{
    //formula based on http://www.cerebralmeltdown.com/programming_projects/Altitude%20and%20Azimuth%20to%20Vector/index.html
    
    //TODO: Have these take higher bitspaces into account, including floating point.
    qreal halfvalue = 128;
    qreal maxvalue = 255;
    
    //have the azimuth and altitude in degrees.
    qreal direction = KisPaintInformation::tiltDirection(info, true)*360.0;
    if (directionType()==0) {
        direction = KisPaintInformation::tiltDirection(info, true)*360.0;
    } else if (directionType()==1) {
        direction = (0.75 + info.drawingAngle() / (2.0 * M_PI))*360.0;
    } else if (directionType()==2) {
        direction = info.rotation();
    }
    qreal elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    
    //subtract/add the rotation of the canvas.
    
    if (info.canvasRotation()!=m_canvasAngle && info.canvasMirroredH()==m_canvasAxisXMirrored) {
       m_canvasAngle=info.canvasRotation();
    }
    
    direction = direction-m_canvasAngle;

    //limit the direction/elevation
    
    qreal elevationFull = 90-((90-elevationMax())+elevationMin());
    if (elevationFull<0) {elevationFull = 10;}
    if (elevation>elevationMax()) {
        elevation=elevationMax();
    }
    elevation=elevation-elevationMin();
    if (elevation<0) {elevation = 0;}
    qreal elevationT = ((elevation/elevationFull)*90.0);
    elevation = static_cast<int>(elevationT);
    
    /* this doesn't work...
    qreal directionFull = 360-((360-directionMax())+directionMin());
    if (directionFull<0) {directionFull = 10;}
    if (direction>directionMax()) {
        direction=directionMax();
    }
    direction=direction-directionMin();
    if (direction<0) {direction = 0;}
    qreal directionT = ((direction/directionFull)*360.0);
    direction = static_cast<int>(directionT);
    */
    
    //convert to radians.
    //TODO: Convert this to kis_global's radian function.
    direction = direction*M_PI / 180.0;
    elevation = elevation*M_PI / 180.0;
    
    
    //make variables for axes for easy switching later on.
    qreal horizontal, vertical, depth;
    
    //spherical coordinates always center themselves around the origin, leading to values. We need to work around those...
    
    horizontal = cos(elevation)*sin(direction);
    if (horizontal>0.0) {
        horizontal= halfvalue+(fabs(horizontal)*halfvalue);
    }
    else {
        horizontal= halfvalue-(fabs(horizontal)*halfvalue);
        
    }
    vertical = cos(elevation)*cos(direction);
    if (vertical>0.0) {
        vertical = halfvalue+(fabs(vertical)*halfvalue);
    }
    else {
        vertical = halfvalue-(fabs(vertical)*halfvalue);
    }
    
    if (m_canvasAxisXMirrored && info.canvasMirroredH()) {horizontal = maxvalue-horizontal;}
    if (m_canvasAxisYMirrored && info.canvasMirroredH()) {vertical = maxvalue-vertical;}
    
    depth = sin(elevation)*maxvalue;
    
    //assign right components to correct axes.
    swizzleAssign(horizontal, vertical, depth, r, redChannel(), maxvalue);
    swizzleAssign(horizontal, vertical, depth, g, greenChannel(), maxvalue);
    swizzleAssign(horizontal, vertical, depth, b, blueChannel(), maxvalue);
    
}

/*settings*/
void KisTangentTiltOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(TANGENT_RED, redChannel());
    setting->setProperty(TANGENT_GREEN, greenChannel());
    setting->setProperty(TANGENT_BLUE, blueChannel());
    setting->setProperty(TANGENT_TYPE, directionType());
    setting->setProperty(TANGENT_EV_MIN, elevationMin());
    setting->setProperty(TANGENT_EV_MAX, elevationMax());
    //setting->setProperty(TANGENT_DIR_MIN, directionMin());
    //setting->setProperty(TANGENT_DIR_MAX, directionMax());
}

void KisTangentTiltOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->comboRed->setCurrentIndex(setting->getInt(TANGENT_RED, 0));
    m_options->comboGreen->setCurrentIndex(setting->getInt(TANGENT_GREEN, 2));
    m_options->comboBlue->setCurrentIndex(setting->getInt(TANGENT_BLUE, 4));
    
    if (setting->getInt(TANGENT_TYPE)== 0){
        m_options->optionTilt->setChecked(true);
    }
    else if (setting->getInt(TANGENT_TYPE)== 1) {
        m_options->optionDirection->setChecked(true);
    }
    else if (setting->getInt(TANGENT_TYPE)== 2) {
        m_options->optionRotation->setChecked(true);
    }
    
    m_canvasAngle = setting->getDouble("runtimeCanvasRotation", 0.0);//in degrees please.
    m_canvasAxisXMirrored = setting->getBool("runtimeCanvasMirroredX", false);
    m_canvasAxisYMirrored = setting->getBool("runtimeCanvasMirroredY", false);
    
    m_options->spinElevationMin->setValue(setting->getInt(TANGENT_EV_MIN, 0));
    m_options->spinElevationMax->setValue(setting->getInt(TANGENT_EV_MAX, 90));
    //m_options->spinDirectionMin->setValue(setting->getInt(TANGENT_DIR_MIN, 0));
    //m_options->spinDirectionMax->setValue(setting->getInt(TANGENT_DIR_MAX, 360));
}
