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
    m_checkable = false;
    m_options = new KisSandOpOptionsWidget();
    m_options->hide();
    connect(m_options->radiusSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->inkDepletionCHBox, SIGNAL(clicked(bool)), SIGNAL(sigSettingChanged()));
    connect(m_options->opacity, SIGNAL(clicked(bool)), SIGNAL(sigSettingChanged()));
    connect(m_options->saturation, SIGNAL(clicked(bool)), SIGNAL(sigSettingChanged()));
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



bool KisSandOpOption::inkDepletion() const
{
    return m_options->inkDepletionCHBox->isChecked();
}



bool KisSandOpOption::opacity() const
{
    return m_options->opacity->isChecked();
}


bool KisSandOpOption::saturation() const
{
    return m_options->saturation->isChecked();
}


void KisSandOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(SAND_RADIUS, radius());
    setting->setProperty(SAND_INK_DEPLETION, inkDepletion());
    setting->setProperty(SAND_USE_OPACITY, opacity());
    setting->setProperty(SAND_USE_SATURATION, saturation());
}

void KisSandOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->radiusSpinBox->setValue(setting->getInt(SAND_RADIUS));
    m_options->inkDepletionCHBox->setChecked(setting->getBool(SAND_INK_DEPLETION));
    m_options->opacity->setChecked(setting->getBool(SAND_USE_OPACITY));
    m_options->saturation->setChecked(setting->getBool(SAND_USE_SATURATION));
}


