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

#ifndef KOCMPX_H
#define KOCMPX_H

#include "cmpx_export.h"

#include <QFuture>
#include <QImage>

class QByteArray;

/**
 * KoCmpx is a wrapper class that is designed to provide color management
 * facility for Calligra applications.  It uses calls to libCmpx to provide
 * automatic ICC profile selection, as well as to render a color-managed PDF
 * spool file for printing. In addition, a print preview file can be generated
 * prior to a submission of a spool file for print.
 */
class CMPX_EXPORT KoCmpx 
{  
public:
    KoCmpx();
    virtual ~KoCmpx();
    
    /**
     * Sets the current profile automatically based on printer settings.
     * (Processing of a best-fit profile will run concurrently in the background.)
     */
    virtual QFuture<void> setAutoProfile(void);
    
protected:
    
    /**
     * Sets the current printer.
     *
     * @param QByteArray cups destination name
     * @return error code indicating success (0) or failure (1) of printer set up
     */
    int setPrinterName(const QByteArray&);
        
    /**
     * Sets the current ICC profile.
     *
     * @param QByteArray file of an ICC profile (either as a path or filename)
     * @return error code indicating success of ICC profile set up
     */
    int setProfileName(const QByteArray&);
    
    /**
     * Sets the current file to be rendered.
     *
     * @param QByteArray file path to file to be rendered (pdf)
     * @return error code indicating success of render file set up
     */
    int setRenderFileName(const QByteArray&);
    
    /// Getters for returning the current values of each item.
    QByteArray printerName(void);
    QByteArray profileName(void);
    QByteArray pdfFileName(void);
    
    /// Renders a PDF spool file and tags it with the default profile colorspace.
    int renderSpool(void);
    
    /// Renders a PDF spool file without tagging a colorspace.
    int renderTargetPrintSpool(void);    
   
    /// Returns a profile-rendered image file for use as a print preview     
    QImage createPreviewImage(void);
  
private:
    /// This type of variable is necessary to avoid including 'cmpx.h' in this header.
    class Private;
    Private * const d;
};

#endif