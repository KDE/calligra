 
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
}

KisPrinterProfileChooser::~KisPrinterProfileChooser()
{
}