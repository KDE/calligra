#include "KoCmpx.h"

#include <cmpx.h>
 
#include <KoColorProfile.h>

#include <QtCore>
#include <QImage>
#include <QByteArray>

static const int KoCmpxMaxString = 256;

class KoCmpx::Private
{
public:
    cmpx_cm_t *m_colormanager;
};

KoCmpx::KoCmpx() 
    : d(new Private())
{
    d->m_colormanager = cmpxAPI_initialize();
}

int KoCmpx::setPrinterName(const QByteArray& printerId)
{
    int error = 0;
  
    if (!printerId.isEmpty()) {
        error = cmpxAPI_setCurrentPrinter(&d->m_colormanager, printerId);
    } else {
        error = 1; 
    }
    
    return error;
}

int KoCmpx::setProfileName(const QByteArray& profileName)
{
    int error = 0;
    
    if (!profileName.isEmpty()){
        error = cmpxAPI_setProfile(&d->m_colormanager, profileName.constData());
    } else{
        error = 1;
    }
        
    return error;
}

int KoCmpx::setRenderFileName(const QByteArray& spoolFileName)
{
    int error = 0;
    
    if (!spoolFileName.isEmpty()){
        error = cmpxAPI_setPdfFile(&d->m_colormanager, spoolFileName.constData());
    } else{
        error = 1;
    }
        
    return error;
}

QByteArray KoCmpx::printerName()
{
    int error = 0;
    
    char *name = new char[CMPX_STRING_SIZE];
  
    error = cmpxAPI_getCurrentPrinter(d->m_colormanager, &name);    
    QByteArray nameString(name);
    
    delete [] name;
    
    return nameString;
}

QByteArray KoCmpx::profileName()
{
    int error = 0;
    
    char *name = new char[CMPX_STRING_SIZE];
  
    error = cmpxAPI_getProfile(d->m_colormanager, &name, CMPX_ICC_STRING_NAME);    
    QByteArray nameString(name);
    
    delete [] name;
    
    return nameString;
}

QByteArray KoCmpx::pdfFileName()
{
    int error = 0;
    char *name = new char[KoCmpxMaxString];
  
    error = cmpxAPI_getPdfFile(d->m_colormanager, &name);   
    QByteArray nameString(name);
    
    delete [] name;
    
    return nameString;
}

QFuture<void> KoCmpx::autoProfile()
{
    // Automatic profile selection takes a little time at the moment.
    return QtConcurrent::run(cmpxAPI_setAutoProfile, &d->m_colormanager);    
}

int KoCmpx::renderSpool()
{
    int error = cmpxAPI_setSpoolPdf(&d->m_colormanager, CMPX_RENDERMODE_NORMAL);    
    return error;
}

int KoCmpx::renderTargetPrintSpool()
{
    int error = cmpxAPI_setSpoolPdf(&d->m_colormanager, CMPX_RENDERMODE_TARGETPRINT);    
    return error;
}

QImage KoCmpx::createPreviewImage()
{
    QImage previewImage;
  
    int error = 0;
    
    char *renderedPreviewFileName = new char[KoCmpxMaxString];  
    error = cmpxAPI_getPreviewImage(d->m_colormanager, &renderedPreviewFileName, 1, 0);
    
    if (!error)
        previewImage = QImage(QString(renderedPreviewFileName), 0);
    
    delete [] renderedPreviewFileName;
    
    return previewImage;
}

KoCmpx::~KoCmpx()
{
    cmpxAPI_close(&d->m_colormanager);
    delete d;
}
