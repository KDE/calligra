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
#include <QColor>
#include <QPoint>

#include "ui_wdgtangenttiltoption.h"

#include "kis_global.h"
#include <kstandarddirs.h>
#include "kis_factory2.h"

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

    m_options->sliderElevationSensitivity->setRange(0, 100, 0);
    m_options->sliderElevationSensitivity->setValue(100);
    m_options->sliderElevationSensitivity->setSuffix("%");

    m_options->sliderMixValue->setRange(0, 100, 0);
    m_options->sliderMixValue->setValue(50);
    m_options->sliderMixValue->setSuffix("%");
    //TODO: this can be changed in frameworks to  KGlobal::dirs()->findResource("kis_images", "krita-tangetnormal.png");
    QString fileName = KisFactory::componentData().dirs()->findResource("kis_images", "krita-tangentnormal-preview.png");
    QImage preview = QImage(fileName);
    m_options->TangentTiltPreview->setPixmap(QPixmap::fromImage(preview.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    connect(m_options->comboRed, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));
    connect(m_options->comboGreen, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));
    connect(m_options->comboBlue, SIGNAL(currentIndexChanged(int)), SLOT(emitSettingChanged()));

    connect(m_options->optionTilt, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    connect(m_options->optionDirection, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    connect(m_options->optionRotation, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));
    connect(m_options->optionMix, SIGNAL(toggled(bool)), SLOT(emitSettingChanged()));

    connect(m_options->sliderElevationSensitivity, SIGNAL(valueChanged(qreal)), SLOT(emitSettingChanged()));
    connect(m_options->sliderMixValue, SIGNAL(valueChanged(qreal)), SLOT(emitSettingChanged()));
    m_options->sliderMixValue->setVisible(false);

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

QImage KisTangentTiltOption::swizzleTransformPreview (QImage preview)
{
    int width = preview.width();
    int height = preview.height();
    QImage endPreview(preview.width(),preview.height(),QImage::Format_RGB32);
    for (int y=0; y<height; y++) {
	for (int x=0; x<width; x++) {
	    QColor currentcolor = QColor(preview.pixel(x,y));
	    int r, g, b =0;
	    r = previewTransform(currentcolor.red(), currentcolor.green(), currentcolor.blue(), redChannel(), 255);
	    g = previewTransform(currentcolor.red(), currentcolor.green(), currentcolor.blue(), greenChannel(), 255);
	    b = previewTransform(currentcolor.red(), currentcolor.green(), currentcolor.blue(), blueChannel(), 255);
	    QRgb transformedColor = qRgb(r,g,b);
	    endPreview.setPixel(x,y, transformedColor);
	}
    }
    return endPreview;
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
    else if (m_options->optionMix->isChecked()==true) {
        type=3;
    }
    else {
        qWarning()<<"There's something odd with the radio buttons. We'll use Tilt";
    }

    return type;
}

double KisTangentTiltOption::elevationSensitivity() const
{
    return m_options->sliderElevationSensitivity->value(); 
}

double KisTangentTiltOption::mixValue() const
{
    return m_options->sliderMixValue->value();
}
//simplified function for the preview.
int KisTangentTiltOption::previewTransform(int const horizontal, int const vertical, int const depth, int index, int maxvalue)
{
    int component = 0;
    switch(index) {
    case 0: component = horizontal; break;
    case 1: component = maxvalue-horizontal; break;
    case 2: component = vertical; break;
    case 3: component = maxvalue-vertical; break;
    case 4: component = depth; break;
    case 5: component = maxvalue-depth; break;
    }
    return component;
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

    //TODO: Have these take higher bitspaces into account, including floating point. Does that even make any sense?
    qreal halfvalue = 128;
    qreal maxvalue = 255;

    //have the azimuth and altitude in degrees.
    qreal direction = KisPaintInformation::tiltDirection(info, true)*360.0;
    qreal elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    if (directionType()==0) {
        direction = KisPaintInformation::tiltDirection(info, true)*360.0;
	elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    } else if (directionType()==1) {
        direction = (0.75 + info.drawingAngle() / (2.0 * M_PI))*360.0;
	elevation= 0;//turns out that tablets that don't support tilt just return 90 degrees for elevation.
    } else if (directionType()==2) {
        direction = info.rotation();
	elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);//artpens have tilt-recognition, so this should work.
    } else if (directionType()==3) {//mix of tilt+direction, TODO.
	qreal mixamount = mixValue()/100.0;
        direction = (KisPaintInformation::tiltDirection(info, true)*360.0*(1.0-mixamount))+((0.75 + info.drawingAngle() / (2.0 * M_PI))*360.0*(mixamount));
	elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    }

    //subtract/add the rotation of the canvas.

    if (info.canvasRotation()!=m_canvasAngle && info.canvasMirroredH()==m_canvasAxisXMirrored) {
       m_canvasAngle=info.canvasRotation();
    }

    direction = direction-m_canvasAngle;

    //limit the direction/elevation

    //qreal elevationMax = (elevationSensitivity()*90.0)/100.0;
    qreal elevationT = elevation*(elevationSensitivity()/100.0)+(90-(elevationSensitivity()*90.0)/100.0);
    elevation = static_cast<int>(elevationT);

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
    setting->setProperty(TANGENT_EV_SEN, elevationSensitivity());
    setting->setProperty(TANGENT_MIX_VAL, mixValue());
}

void KisTangentTiltOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->comboRed->setCurrentIndex(setting->getInt(TANGENT_RED, 0));
    m_options->comboGreen->setCurrentIndex(setting->getInt(TANGENT_GREEN, 2));
    m_options->comboBlue->setCurrentIndex(setting->getInt(TANGENT_BLUE, 4));

    if (setting->getInt(TANGENT_TYPE)== 0){
        m_options->optionTilt->setChecked(true);
	m_options->sliderMixValue->setVisible(false);
    }
    else if (setting->getInt(TANGENT_TYPE)== 1) {
        m_options->optionDirection->setChecked(true);
	m_options->sliderMixValue->setVisible(false);
    }
    else if (setting->getInt(TANGENT_TYPE)== 2) {
        m_options->optionRotation->setChecked(true);
	m_options->sliderMixValue->setVisible(false);
    }
    else if (setting->getInt(TANGENT_TYPE)== 3) {
        m_options->optionMix->setChecked(true);
	m_options->sliderMixValue->setVisible(true);
    }

    m_canvasAngle = setting->getDouble("runtimeCanvasRotation", 0.0);//in degrees please.
    m_canvasAxisXMirrored = setting->getBool("runtimeCanvasMirroredX", false);
    m_canvasAxisYMirrored = setting->getBool("runtimeCanvasMirroredY", false);

    m_options->sliderElevationSensitivity->setValue(setting->getDouble(TANGENT_EV_SEN, 100));
    m_options->sliderMixValue->setValue(setting->getDouble(TANGENT_MIX_VAL, 50));

    QString fileName = KisFactory::componentData().dirs()->findResource("kis_images", "krita-tangentnormal-preview.png");
    QImage preview = QImage(fileName);
    preview = swizzleTransformPreview (preview);
    m_options->TangentTiltPreview->setPixmap(QPixmap::fromImage(preview.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    m_options->TangentTiltPreview->setUpdatesEnabled(true);
    m_options->update();

}
