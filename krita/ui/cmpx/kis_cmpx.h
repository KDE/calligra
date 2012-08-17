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

#include <KoCmpx.h>

#include <QPrinter>
#include <QPrinterInfo>
#include <QImage>
#include <QFuture>
#include <kis_types.h>

#include "krita_export.h"

class KoColorProfile;
class KoColorSpace;

/** KisCmpx 
 * 
 * @brief Krita wrapper class for libCmpx (Library for Color-Managed Printer eXtension)
 * 
 * Inherits KoCmpx and provides an interface for handling color-managed printing 
 * duties within Krita.
 * 
 * @see /libs/cmpx/KoCmpx.h
 */
class KRITAUI_EXPORT KisCmpx : public KoCmpx
{        
public:    
    KisCmpx();
    
    /**
     * @return file path of the currently set pdf render file
     */
    virtual QString pdfFile(void);  
    
    /**
     * @return currently set printer
     */
    virtual QPrinter * printer(void);
    
    /**
     * @return currently set icc profile
     */
    virtual const KoColorProfile * profile(void);
    
    /**
     * Sets the location of a PDF file for rendering.
     * 
     * @param QString file path to PDF file
     * @return 0 for success and 1 for error
     */
    virtual int setPdfFile(const QString);
    
    /**
     * Sets the printer.
     * 
     * @param QPrinter an installed printer
     * @return 0 for success and 1 for error
     */
    virtual int setPrinter(const QPrinter *);
    
    /**
     * Sets the profile based on Krita color space.
     * 
     * @param KoColorSpace valid krita color space reference
     * @return 0 for success and 1 for error
     */
    virtual int setProfile(const KoColorSpace *);
    
    /**
     * Sets the profile based on Krita profile.
     * 
     * @param KoColorSpace valid krita profile reference
     * @return 0 for success and 1 for error
     */
    virtual int setProfile(const KoColorProfile *);
    
    /**
     * Sets the profile based on the currently stored printer.
     * 
     * @return concurrently running state of profile ranking process
     */
    virtual QFuture<void> setAutoProfile();  
    
    /**
     * Creates a color-managed PDF spool file. If a profile is specified,
     * then the default color space will also be tagged in the PDF file.  
     * 
     * @param KisImageWSP krita image 
     * @param KoColorSpace profile to embed
     * @return path to the rendered pdf file or an empty string if job fails
     */
    virtual QString renderSpoolPdf(KisImageWSP, const KoColorProfile *);
    
    /**
     * Creates a simulated color print preview of an image using an icc profile.
     * 
     * @param KisImageWSP krita image 
     * @param KoColorSpace profile to embed
     * @return icc embedded image
     */
    virtual QImage renderPreviewImage(KisImageWSP, const KoColorProfile *);
    
protected:       
    /// Register/load a printer profile from Krita's configuration.
    virtual void saveProfile(void);
    virtual void loadProfile(void);
    
    /// Helper to convert a Krita image to a PDF file.
    virtual QString convertImageToPdf(KisImageWSP, const QString);
};

#endif