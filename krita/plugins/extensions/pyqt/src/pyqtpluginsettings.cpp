/*
 *  Copyright (c) 2014 Boudewijn Rempt <boud@kogmbh.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "pyqtpluginsettings.h"

#include "ui_manager.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <kconfiggroup.h>

#include <KoIcon.h>


#include "kis_config.h"


PyQtPluginSettings::PyQtPluginSettings(QWidget *parent) :
        KisPreferenceSet(parent),
        ui(new Ui::ManagerPage)
{
    ui->setupUi(this);
}

PyQtPluginSettings::~PyQtPluginSettings()
{
    delete ui;
}

QString PyQtPluginSettings::id()
{
    return QString("pyqtpluginmanager");
}

QString PyQtPluginSettings::name()
{
    return header();
}

QString PyQtPluginSettings::header()
{
    return QString(i18n("Python Plugin Manager"));
}


KIcon PyQtPluginSettings::icon()
{
    return koIcon("applications-development");
}


void PyQtPluginSettings::savePreferences() const
{
    emit settingsChanged();
}

void PyQtPluginSettings::loadPreferences()
{
}

void PyQtPluginSettings::loadDefaultPreferences()
{
}
