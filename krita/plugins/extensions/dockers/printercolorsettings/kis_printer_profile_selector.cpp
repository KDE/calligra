 
#include "kis_printer_profile_selector.h"

#include <KoColorProfile.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorSpaceEngine.h>
#include <KoID.h>

#include <KoConfig.h>

KisPrinterProfileChooser::KisPrinterProfileChooser(QWidget *parent) : 
    KisColorSpaceSelector(parent)
{   
    connect(m_ui.printerProfilesCheckBox, SIGNAL(toggled(bool)), 
            this, SLOT(slotNarrowProfileSelection(bool)));    
}

/// Narrows list of profiles down to print-related choices in "Profile Chooser".
void KisPrinterProfileChooser::slotNarrowProfileSelection(bool checkState)
{
    bool hiddenState = checkState;
  
    m_ui.cmbColorModels->setHidden(hiddenState);
    m_ui.cmbColorDepth->setHidden(hiddenState);
    m_ui.lblColorModels->setHidden(hiddenState);
    m_ui.lblColorSpaces->setHidden(hiddenState);
    
    if (checkState = true) {
        // TODO Narrow down choices to print-related profiles.
    }
    
}

KisPrinterProfileChooser::~KisPrinterProfileChooser()
{
}