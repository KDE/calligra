/*
 *  Copyright (c) 2012 Francisco Fernandes <francisco.fernandes.j@gmail.com>
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

#include "kis_sandop_option.h"

#include "ui_wdgsandoptions.h"

class KisSandOpOptionsWidget: public QWidget, public Ui::WdgSandOptions
{
public:
    KisSandOpOptionsWidget(QWidget *parent = 0)
            : QWidget(parent) {
        setupUi(this);
    }
};

KisSandOpOption::KisSandOpOption()
        : KisPaintOpOption(i18n("Brush size"), KisPaintOpOption::brushCategory(), false)
{
    m_checkable = false; //??
    m_options = new KisSandOpOptionsWidget();
    m_options->hide();
    
    connect(m_options->radiusSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->amountSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->sandDepletionCHBox, SIGNAL(clicked(bool)), SIGNAL(sigSettingChanged()));

//Sand spread mode signals and slots
//     connect(m_options->modeCHBox, SIGNAL(clicked(bool)), SIGNAL(sigSettingChanged()));
//     connect(m_options->massSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
//     connect(m_options->frictionSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
//     connect(m_options->dissipationSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));

    setConfigurationPage(m_options);
}

KisSandOpOption::~KisSandOpOption()
{
    // delete m_options;
}

int KisSandOpOption::radius() const
{
    return m_options->radiusSpinBox->value();
}


void KisSandOpOption::setRadius(int radius) const
{
    m_options->radiusSpinBox->setValue( radius );
}

int KisSandOpOption::amount() const
{
    return m_options->amountSpinBox->value();
}


void KisSandOpOption::setAmount(int amount) const
{
    m_options->amountSpinBox->setValue( amount );
}


bool KisSandOpOption::sandDepletion() const
{
    return m_options->sandDepletionCHBox->isChecked();
}


/*
 * SPREAD MODE FUNCTIONS:
 * FIX THESE FUNCTIONS FOR WORK WITH PROPER FLOAT WIDGETS
 */


// bool KisSandOpOption::mode() const
// {
//     return m_options->modeCHBox->isChecked();
// }
// float KisSandOpOption::friction() const
// {
//     return m_options->frictionSpinBox->value();
// }
// float KisSandOpOption::mass() const
// {
//     return m_options->massSpinBox->value();
// }
//
// void KisSandOpOption::setMass(float mass) const
// {
//     m_options->massSpinBox->setValue( mass );
// }
// 
// 
// void KisSandOpOption::setFriction(float friction) const
// {
//     m_options->frictionSpinBox->setValue( friction );
// }
// 
// float KisSandOpOption::dissipation() const
// {
//     return m_options->dissipationSpinBox->value();
// }
// 
// void KisSandOpOption::setFriction(float dissipation) const
// {
//     m_options->dissipationSpinBox->setValue( dissipation );
// }



void KisSandOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(SAND_RADIUS, radius());
    setting->setProperty(SAND_AMOUNT, amount());
    setting->setProperty(SAND_DEPLETION, sandDepletion());

//Sand spread mode write settings
//     setting->setProperty(SAND_MODE, mode());
//     setting->setProperty(SAND_MASS, mass());
//     setting->setProperty(SAND_FRICTION, sandFriction());
//     setting->setProperty(SAND_DISSIPATION, sandDissipation());
}

void KisSandOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->radiusSpinBox->setValue(setting->getInt(SAND_RADIUS));
    m_options->amountSpinBox->setValue(setting->getInt(SAND_AMOUNT));
    m_options->sandDepletionCHBox->setChecked(setting->getBool(SAND_DEPLETION));

//Sand spread mode read settings
//     m_options->modeCHBox->setChecked(setting->getBool(SAND_MODE));
//     m_options->massSpinBox->setValue(setting->getInt(SAND_MASS));
//     m_options->frictionSpinBox->setValue(setting->getInt(SAND_FRICTION));
//     m_options->dissipationSpinBox->setValue(setting->getInt(SAND_DISSIPATION));
    
}