/*
 *  Copyright (c) 2012 José Luis Vergara <pentalis@gmail.com>
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
#include "kis_quickpix_paintop_options.h"

#include "ui_wdgquickpixoptions.h"

class KisQuickPixOpOptionsWidget: public QWidget, public Ui::WdgQuickPixOptions
{
public:
    KisQuickPixOpOptionsWidget(QWidget *parent = 0)
            : QWidget(parent) {
        setupUi(this);
    }
};

KisQuickPixOpOption::KisQuickPixOpOption()
        : KisPaintOpOption(i18n("Brush size"), KisPaintOpOption::brushCategory(), false)
{
    m_checkable = false;
    m_options = new KisQuickPixOpOptionsWidget();
    connect(m_options->exampleWidget, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));

    setConfigurationPage(m_options);
}

KisQuickPixOpOption::~KisQuickPixOpOption()
{
    // delete m_options;
}

int KisQuickPixOpOption::exampleSetting() const
{
    return m_options->exampleWidget->value();
}


void KisQuickPixOpOption::setExampleSetting(int exampleSetting) const
{
    m_options->exampleWidget->setValue( exampleSetting );
}


void KisQuickPixOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(EXAMPLE_SETTING, exampleSetting());
}

void KisQuickPixOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->exampleWidget->setValue(setting->getInt(EXAMPLE_SETTING));
}


