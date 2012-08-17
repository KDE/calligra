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

#ifndef KISCMPX_H
#define KISCMPX_H

#include <QPrinter>
#include <QPrinterInfo>
#include <QImage>
#include <QFuture>
#include <kis_types.h>

//#include <KoCmpx.h>

#include <cmpx.h>

class KoColorProfile;

/// Krita wrapper class for libCmpx (Library for Color-Managed Printer eXtensions)
class KisCmpx 
{
public:
    KisCmpx();
    virtual ~KisCmpx();
    
    virtual QString pdfFile(void);
    virtual QString printerName(void);
    virtual QString profileName(void);    
    
    virtual QPrinter * printer(void);
    virtual const KoColorProfile * profile(void);
    
    virtual int setPdfFile(const QString);
    virtual int setPrinter(const QPrinter *);
    virtual int setProfile(const KoColorProfile *);        
    
    QFuture<void> setAutoProfile();  
    
    virtual QString renderSpoolPdf(KisImageWSP, const KoColorProfile *);
    virtual QImage renderPreviewImage(KisImageWSP, const KoColorProfile *);
    
protected:           
    virtual QString convertImageToPdf(KisImageWSP, const QString);
    
private:
    cmpx_cm_t *m_colormanager;
};

#endif