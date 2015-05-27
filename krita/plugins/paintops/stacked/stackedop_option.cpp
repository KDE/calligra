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
#include "stackedop_option.h"

#include "ui_wdgstackedoptions.h"

class StackedOpOptionsWidget: public QWidget, public Ui::WdgStackedOptions
{
public:
    StackedOpOptionsWidget(QWidget *parent = 0)
        : QWidget(parent) {
        setupUi(this);
    }
};

StackedOpOption::StackedOpOption()
    : KisPaintOpOption(KisPaintOpOption::GENERAL, false)
{
    m_checkable = false;
    m_options = new StackedOpOptionsWidget();
    m_options->hide();

    setObjectName("StackedOpOption");

    setConfigurationPage(m_options);
}

StackedOpOption::~StackedOpOption()
{
    // delete m_options;
}

void StackedOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
}

void StackedOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
}


