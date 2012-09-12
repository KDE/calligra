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
    connect(m_options->amountSpinBox, SLOT(setDisabled(bool)), m_options->sandDepletionCHBox, SIGNAL(clicked(bool)));

    connect(m_options->sizeSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->massDoubleSpinBox, SIGNAL(valueChanged(double)), SIGNAL(sigSettingChanged()));
    connect(m_options->frictionDoubleSpinBox, SIGNAL(valueChanged(double)), SIGNAL(sigSettingChanged()));

    //OBS!!
    connect(m_options->modeComboBox, SIGNAL(currentIndexChanged(QString)), SIGNAL(sigSettingChanged()));

    setConfigurationPage(m_options);
}

KisSandOpOption::~KisSandOpOption()
{
    // delete m_options;
}

//Brush particle settings

double KisSandOpOption::bFriction() const
{
    return m_options->bFrictionDoubleSpinBox->value();
}

void KisSandOpOption::setBfriction(double friction) const
{
    m_options->bFrictionDoubleSpinBox->setValue( friction );
}

double KisSandOpOption::bMass() const
{
    return m_options->bMassDoubleSpinBox->value();
}

void KisSandOpOption::setBmass(double mass) const
{
    m_options->bMassDoubleSpinBox->setValue( mass );
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
bool KisSandOpOption::mode() const
{
    //if the combo box has selected "Pouring"
    if(m_options->modeComboBox->currentText() == "Pouring"){
        m_options->bMassDoubleSpinBox->setDisabled(true);
        m_options->bFrictionDoubleSpinBox->setDisabled(true);

        return false;
    }

    m_options->bMassDoubleSpinBox->setEnabled(true);
    m_options->bFrictionDoubleSpinBox->setEnabled(true);
    //if the combo box has selected "Spread"
    return true;
    
//     return m_options->modeCHBox->isChecked();
}

int KisSandOpOption::size() const
{
    return m_options->sizeSpinBox->value();
}

void KisSandOpOption::setSize(int size) const
{
    m_options->sizeSpinBox->setValue( size );
}

//Particles settings

double KisSandOpOption::friction() const
{
    return m_options->frictionDoubleSpinBox->value();
}

void KisSandOpOption::setFriction(double friction) const
{
    m_options->frictionDoubleSpinBox->setValue( friction );
}

double KisSandOpOption::mass() const
{
    return m_options->massDoubleSpinBox->value();
}

void KisSandOpOption::setMass(double mass) const
{
    m_options->massDoubleSpinBox->setValue( mass );
}


// double KisSandOpOption::dissipation() const
// {
//     return m_options->dissipationSpinBox->value();
// }

// void KisSandOpOption::setDissipation(double dissipation) const
// {
//     m_options->dissipationSpinBox->setValue( dissipation );
// }

void KisSandOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(SAND_RADIUS, radius());
    setting->setProperty(SAND_AMOUNT, amount());
    setting->setProperty(SAND_DEPLETION, sandDepletion());

    setting->setProperty(SAND_SIZE, size());
    setting->setProperty(SAND_MASS, mass());
    setting->setProperty(SAND_FRICTION, friction());

    setting->setProperty(SAND_BMASS, bMass());
    setting->setProperty(SAND_BFRICTION, bFriction());

//Sand spread mode write settings
    setting->setProperty(SAND_MODE, mode());
//     setting->setProperty(SAND_DISSIPATION, sandDissipation());
}



void KisSandOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->radiusSpinBox->setValue(setting->getInt(SAND_RADIUS));
    m_options->amountSpinBox->setValue(setting->getInt(SAND_AMOUNT));
    m_options->sandDepletionCHBox->setChecked(setting->getBool(SAND_DEPLETION));

    m_options->sizeSpinBox->setValue(setting->getInt(SAND_SIZE));
    m_options->massDoubleSpinBox->setValue(setting->getDouble(SAND_MASS));
    m_options->frictionDoubleSpinBox->setValue(setting->getDouble(SAND_FRICTION));

    m_options->bMassDoubleSpinBox->setValue(setting->getDouble(SAND_BMASS));
    m_options->bFrictionDoubleSpinBox->setValue(setting->getDouble(SAND_BFRICTION));
    
//     m_options->modeCHBox->setChecked(setting->getBool(SAND_MODE));
    
    if(setting->getBool(SAND_MODE)){
        m_options->modeComboBox->setCurrentIndex(0); //pouring
    }else{
        m_options->modeComboBox->setCurrentIndex(1); //spread
    }
    
//     m_options->dissipationSpinBox->setValue(setting->getInt(SAND_DISSIPATION));   
}
