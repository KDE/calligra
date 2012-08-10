/*
 *  Copyright (c) 2012 Joseph Simon <jsimon383@gmail.com>
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

#include "kis_printer_color_manager.h"
#include "cmpx/kis_cmpx.h"
#include "kis_printer_profile_selector.h"

#include <QPainter>
#include <QPrinter>

#include <KoColorSpaceRegistry.h>
#include <KoColorProfile.h>
#include <KoColorSpace.h>

#include <kis_image.h>

#include <stdio.h>

KisPrinterColorManager::KisPrinterColorManager(QWidget *parent) 
    : QWidget(parent)
{    
    setupUi(this);
    
    m_colormanager = new KisCmpx();
    m_selectorui = new KisPrinterProfileChooser(profileSelectorWidget);
         
    QString printerProfileName = m_cfg.readEntry("kiscmpx::printer.profile", QString(""));
    m_profile = KoColorSpaceRegistry::instance()->profileByName(printerProfileName);
    
    label->setText(printerProfileName);
    
    populatePrinterList();
    
    //m_image = new KisImage(0, 100, 100, 0, "dummy", true);
    
    bool autoProfileCheckState = m_cfg.readEntry("kiscmpx::autoprofile.checked", false);
    emit slotSetAutoCheckBox(autoProfileCheckState);
    
    if (autoProfileCheckState == false) {
       setCurrentProfile(m_profile);
    }    
    
    connect(printerListComboBox, SIGNAL(currentIndexChanged(int)), 
            this, SLOT(slotChangePrinterSelection(int)));
    connect(autoProfileCheckBox, SIGNAL(toggled(bool)), 
            this, SLOT(slotSetAutoCheckBox(bool)));
    connect(&m_watcher, SIGNAL(finished()), 
            this, SLOT(slotFinishedAutoProfile()));
    connect(m_selectorui, SIGNAL(colorSpaceChanged(const KoColorSpace*)), 
            this, SLOT(slotUserProfileChanged(const KoColorSpace*)));
    //connect(printButton, SIGNAL(clicked()), 
    //        this, SLOT(slotPrint()));
}

#if 0
void KisPrinterColorManager::slotPrint()
{
    //simulatePrintJob();
}
#endif

void KisPrinterColorManager::slotUserProfileChanged(const KoColorSpace* colorSpace)
{
    const KoColorProfile * profile = colorSpace->profile();
    setCurrentProfile(profile);
}

void KisPrinterColorManager::setCurrentProfile(const KoColorProfile *profile)
{
    int error = m_colormanager->setProfile(profile);
    
    if (error) {
        label->setText("*Unable to set profile*"); 
    } else {
        label->setText(m_colormanager->profileName());
        m_profile = profile;

        registerCurrentProfile();
    } 
}

void KisPrinterColorManager::slotChangePrinterSelection(int index)
{  
    QPrinter selectedPrinter(m_printerlist.at(index), QPrinter::ScreenResolution);       
    
    m_colormanager->setPrinter(&selectedPrinter);    
    m_currentprinter = &selectedPrinter;
    
    emit slotSetAutoCheckBox(autoProfileCheckBox->isChecked());
}

void KisPrinterColorManager::slotSetAutoCheckBox(bool checkState)
{
    if (checkState == true) {
        m_watcher.setFuture(m_colormanager->setAutoProfile(m_currentprinter));
        label->setText("Searching for profile...");

        m_selectorui->setHidden(true);
        autoProfileWarningLabel->setHidden(false);
    } else if (checkState == false) {
        m_selectorui->setHidden(false);
        autoProfileWarningLabel->setHidden(true);
    }
    
    m_cfg.writeEntry("kiscmpx::autoprofile.checked", checkState);
}

void KisPrinterColorManager::slotFinishedAutoProfile(void)
{
    label->setText(m_colormanager->profileName());
}


void KisPrinterColorManager::simulatePrintJob()
{
    QString spoolFile = m_colormanager->renderSpoolPdf(m_image, m_profile);
    // ...
}

void KisPrinterColorManager::registerCurrentProfile(void)
{
    QString profileFileName =  m_profile->fileName();
    m_cfg.writeEntry("kiscmpx::printer.profile", profileFileName);
}

void KisPrinterColorManager::populatePrinterList(void)
{    
    int defaultPrinterIndex;
    QString printerName;
    QString defaultPrinterName;       
    QPrinterInfo currentPrinter;

    m_printerlist = QPrinterInfo::availablePrinters();    

    for (int i = 0; i < m_printerlist.count(); i++) { 
        currentPrinter = m_printerlist.at(i);  
        printerName = currentPrinter.printerName();

        printerListComboBox->addItem(printerName, 0);
    }
    
    defaultPrinterName = QPrinterInfo::defaultPrinter().printerName();
    defaultPrinterIndex = printerListComboBox->findText(defaultPrinterName, Qt::MatchExactly);
    
    m_currentprinter = new QPrinter(m_printerlist.at(0), QPrinter::ScreenResolution);  
    
    emit slotChangePrinterSelection(defaultPrinterIndex);
}

KisPrinterColorManager::~KisPrinterColorManager()
{   
}

#include "kis_printer_color_manager.moc"