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
#include "kis_lindenmayerop_option.h"

#include <QMetaType>
#include "kis_lindenmayer_script_error_repeater.h"

#include "ui_wdglindenmayeroptions.h"
#include <QDebug>

class KisLindenmayerOpOptionsWidget: public QWidget, public Ui::WdgLindenmayerOptions
{
public:
    KisLindenmayerOpOptionsWidget(QWidget *parent = 0)
            : QWidget(parent) {
        setupUi(this);
        this->splitter->setCollapsible(0, false);
    }
};

KisLindenmayerOpOption::KisLindenmayerOpOption()
        : KisPaintOpOption(i18n("Brush size"), KisPaintOpOption::brushCategory(), false)
{
    m_checkable = false;
    m_options = new KisLindenmayerOpOptionsWidget();
    connect(m_options->radiusSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->minDistSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->maxDistSpinBox, SIGNAL(valueChanged(int)), SIGNAL(sigSettingChanged()));
    connect(m_options->codeEditor, SIGNAL(textChanged()), SIGNAL(sigSettingChanged()));
    connect(m_options->codeEditor, SIGNAL(textChanged()), SLOT(clearScriptErrors()));

    qRegisterMetaType<QList<QPair<int,QString> > >("QList<QPair<int,QString> >");
    connect(KisLindenmayerScriptErrorRepeater::instance(), SIGNAL(errorsOccured(QList<QPair<int,QString> >)), SLOT(setScriptErrors(QList<QPair<int,QString> >)), Qt::UniqueConnection);

    setConfigurationPage(m_options);
}

KisLindenmayerOpOption::~KisLindenmayerOpOption()
{
    // delete m_options;
}

int KisLindenmayerOpOption::radius() const
{
    return m_options->radiusSpinBox->value();
}


void KisLindenmayerOpOption::setRadius(int radius) const
{
    m_options->radiusSpinBox->setValue( radius );
}

QString KisLindenmayerOpOption::code() const
{
    return m_options->codeEditor->toPlainText();
}

int KisLindenmayerOpOption::minDistance() const {
    return m_options->minDistSpinBox->value();
}

int KisLindenmayerOpOption::maxDistance() const {
    return m_options->maxDistSpinBox->value();
}

void KisLindenmayerOpOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
    setting->setProperty(LINDENMAYER_RADIUS, radius());
    setting->setProperty(LINDENMAYER_CODE, code().replace(QRegExp("\n"), "<br>"));
    setting->setProperty(LINDENMAYER_MIN_DISTANCE, minDistance());
    setting->setProperty(LINDENMAYER_MAX_DISTANCE, maxDistance());
}

void KisLindenmayerOpOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
    m_options->radiusSpinBox->setValue(setting->getInt(LINDENMAYER_RADIUS));
    m_options->codeEditor->setPlainText(setting->getString(LINDENMAYER_CODE).replace("<br>", QString('\n')));
    m_options->minDistSpinBox->setValue(setting->getInt(LINDENMAYER_MIN_DISTANCE));
    m_options->maxDistSpinBox->setValue(setting->getInt(LINDENMAYER_MAX_DISTANCE));
}

void KisLindenmayerOpOption::setScriptErrors(QList<QPair<int, QString> > errors) {
    m_options->errorTableWidget->setRowCount(errors.size());
    QPair<int, QString> pair;
    int i=0;
    foreach (pair, errors) {
        int rowNumber = i;
        i++;

        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(pair.first));
        m_options->errorTableWidget->setItem(rowNumber, 0, newItem);

        newItem = new QTableWidgetItem(pair.second);
        m_options->errorTableWidget->setItem(rowNumber, 1, newItem);
    }
}

void KisLindenmayerOpOption::clearScriptErrors() {
    m_options->errorTableWidget->setRowCount(0);
}

void KisLindenmayerOpOption::setScriptToManyLettersError() {
    // will be used later to indicate, that the productions are producing too many letters/items
}


