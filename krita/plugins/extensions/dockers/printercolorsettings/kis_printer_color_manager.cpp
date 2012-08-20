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
#include "kis_printer_profile_selector.h"

#include <cmpx/kis_cmpx.h>

#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QMessageBox>

#include <KoColorSpaceRegistry.h>
#include <KoColorProfile.h>
#include <KoColorSpace.h>

#include <kis_image.h>

KisPrinterColorManager::KisPrinterColorManager(QWidget *parent) 
    : QWidget(parent)
{    
    setupUi(this);
    
    int printerCount = populatePrinterList();
    
    // If no printer is detected, we disable the color manager.
    if (printerCount < 1) {
        mainTabWidget->setEnabled(false);
        activeProfileLabel->setText("**Printer not detected**  Printer color management disabled.");
    } else {    
        m_colormanager = new KisCmpx();
        m_selectorui = new KisPrinterProfileChooser(profileSelectorWidget);
   
        KisConfig mc;
        QString printerProfileName = mc.printerProfile();
        m_profile = KoColorSpaceRegistry::instance()->profileByName(printerProfileName);
            
        if (m_profile) {
            setCurrentProfile(m_profile);
        }
    }
    
    connect(printerListComboBox, SIGNAL(currentIndexChanged(int)), 
            this, SLOT(slotChangePrinterSelection(int)));
    connect(autoProfileCheckBox, SIGNAL(toggled(bool)), 
            this, SLOT(slotSetAutoCheckBox(bool)));
    connect(&m_watcher, SIGNAL(finished()), 
            this, SLOT(slotFinishedAutoProfile()));
    connect(m_selectorui, SIGNAL(colorSpaceChanged(const KoColorSpace*)), 
            this, SLOT(slotUserProfileChanged(const KoColorSpace*)));
    connect(printButton, SIGNAL(clicked()), 
            this, SLOT(slotPrint()));
    connect(refreshButton, SIGNAL(clicked()), 
            this, SLOT(slotRefreshPreview()));
}

/// Clicking the "Print" Button
void KisPrinterColorManager::slotPrint()
{
    //simulatePrintJob();
}

/// Changing the profile from the profile selection combobox.
void KisPrinterColorManager::slotUserProfileChanged(const KoColorSpace* colorSpace)
{
    const KoColorProfile * profile = colorSpace->profile();
    setCurrentProfile(profile);
}

/// Sets a profile for KisCmpx.
void KisPrinterColorManager::setCurrentProfile(const KoColorProfile *profile)
{
    if (!profile) {
        activeProfileLabel->setText("Please set a printer profile."); 
    } else {
        m_colormanager->setProfile(profile);

        activeProfileLabel->setText(profile->name());
        m_profile = profile;
    } 
}

void KisPrinterColorManager::slotChangePrinterSelection(int index)
{  
    QPrinter selectedPrinter(m_printerlist.at(index), QPrinter::ScreenResolution);       
    
    m_colormanager->setPrinter(&selectedPrinter);  
    
    emit slotSetAutoCheckBox(autoProfileCheckBox->isChecked());
}

/// Action performed when the "Automatic Profile Selection" box is checked.
void KisPrinterColorManager::slotSetAutoCheckBox(bool checkState)
{
    if (checkState == true) {
        m_selectorui->setHidden(true);
        autoProfileWarningLabel->setHidden(false);

        activeProfileLabel->setText("Searching for profile...");

        m_watcher.setFuture(m_colormanager->setAutoProfile());
    } else if (checkState == false) {
        m_selectorui->setHidden(false);

        autoProfileWarningLabel->setHidden(true);
    }
}

/// This is called whenever an auto-profile process is complete
void KisPrinterColorManager::slotFinishedAutoProfile(void)
{
    const KoColorProfile * obtainedProfile = m_colormanager->profile();
    setCurrentProfile(obtainedProfile);
}

/// Displays the simulated color preview of an image.
void KisPrinterColorManager::slotRefreshPreview()
{
    // NOTE This needs a little more testing.
    //QImage previewImage = m_colormanager->renderPreviewImage(m_image, m_colormanager->profile());
    //previewLabel->setPixmap(QPixmap::fromImage(previewImage));
}

/// Create a color-managed PDF spool file.
void KisPrinterColorManager::simulatePrintJob()
{ 
    QMessageBox msgBox(this);
    
    QPrinter *currentPrinter = 0;
    const KoColorProfile *currentProfile = 0;
    
    currentPrinter = m_colormanager->printer();
    currentProfile = m_colormanager->profile();
 
    QString pdfFileName = m_colormanager->renderSpoolPdf(m_image, currentProfile);
    
    if (!pdfFileName.isEmpty())
        msgBox.setText("Color-managed PDF spool file set at " + pdfFileName);
    else
        msgBox.setText("Unable to set PDF spool file.");
    
    msgBox.exec();
}

/// Set an image from the view.
void KisPrinterColorManager::setImage(KisImageWSP image)
{
    m_image = image;
}

/// Detects the available printers in the system.
int KisPrinterColorManager::populatePrinterList(void)
{    
    int defaultPrinterIndex;
    QString printerName;
    QString defaultPrinterName;       
    QPrinterInfo currentPrinter;

    m_printerlist = QPrinterInfo::availablePrinters();
    
    int count = m_printerlist.count();

    for (int i = 0; i < count; i++) { 
        currentPrinter = m_printerlist.at(i);  
        printerName = currentPrinter.printerName();

        printerListComboBox->addItem(printerName, 0);
    }
   
    if (count > 0) {    
        defaultPrinterName = QPrinterInfo::defaultPrinter().printerName();
        defaultPrinterIndex = printerListComboBox->findText(defaultPrinterName, Qt::MatchExactly);
    
        QPrinter p(m_printerlist.at(0), QPrinter::ScreenResolution);  
        m_colormanager->setPrinter(&p);

        emit slotChangePrinterSelection(defaultPrinterIndex);
    } 
    
    return count;
}

KisPrinterColorManager::~KisPrinterColorManager()
{   
}

#include "kis_printer_color_manager.moc"