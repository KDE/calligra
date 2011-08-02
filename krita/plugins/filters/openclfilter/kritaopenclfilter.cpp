/*
 * This file is part of Krita
 *
 * Copyright (c) 2011 Matus Talcik <matus.talcik@gmail.com>
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

#include "kritaopenclfilter.h"
#include "kritawidgetopencl.h"

#include <filter/kis_filter_configuration.h>
#include <kis_selection.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>
#include <kis_iterator_ng.h>
#include <kis_math_toolbox.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>

#include <qclcontext.h>

KritaOpenCLFilter::KritaOpenCLFilter() : KisFilter(id(), categoryOther(), i18n("&OpenCL..."))
{
    setSupportsPainting(false);
    setSupportsIncrementalPainting(false);
    setSupportsAdjustmentLayers(false);
    setSupportsThreading(false);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

KisConfigWidget * KritaOpenCLFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP, const KisImageWSP image) const
{
    Q_UNUSED(image)
    return new KritaWidgetOpenCL(parent);
}

KisFilterConfiguration* KritaOpenCLFilter::factoryConfiguration(const KisPaintDeviceSP) const
{
    KisFilterConfiguration* config = new KisFilterConfiguration(id().id(), 1);
    return config;
}

void KritaOpenCLFilter::process(KisPaintDeviceSP device,
                            const QRect& rect,
                            const KisFilterConfiguration* config,
                            KoUpdater* progressUpdater
                           ) const
{
    /* VARIABLES */
    // Basic
    int width = rect.width();
    int height = rect.height();
    int numberOfPixels = width * height;
    
    // Pixels
    QVector<quint8 *> pixels = device->readPlanarBytes(0, 0, width, height);
    
    /* GET KERNEL CODE */
    QVariant text;
    config->getProperty("kernel", text);
    QString kernelCode = text.toString();
    
    if(kernelCode.isEmpty()) {
      return;
    }

    /* CREATE A KERNEL */
    QCLContext context;
    if(!context.create(QCLDevice::GPU)) {
      qFatal("Could not create OpenCL context");
    }
    
    QCLProgram program = context.buildProgramFromSourceCode(qPrintable(kernelCode));
    QCLKernel kritaKernel = program.createKernel("kritaKernel");
    
    /* CREATE DATA STRUCTURES FOR KERNEL */
    QCLBuffer blue  = context.createBufferCopy(pixels[0], numberOfPixels * sizeof(quint8), QCLMemoryObject::ReadWrite);
    QCLBuffer green = context.createBufferCopy(pixels[1], numberOfPixels * sizeof(quint8), QCLMemoryObject::ReadWrite);
    QCLBuffer red   = context.createBufferCopy(pixels[2], numberOfPixels * sizeof(quint8), QCLMemoryObject::ReadWrite);
    QCLBuffer alpha = context.createBufferCopy(pixels[3], numberOfPixels * sizeof(quint8), QCLMemoryObject::ReadWrite);
    
    /* RUN KERNEL */
    kritaKernel.setGlobalWorkSize(numberOfPixels);
    //kritaKernel.setLocalWorkSize();
    kritaKernel(blue, green, red, alpha);
    
    /* RETRIEVE DATA FROM KERNEL */
    blue.read (pixels[0], numberOfPixels * sizeof(quint8));
    green.read(pixels[1], numberOfPixels * sizeof(quint8));
    red.read  (pixels[2], numberOfPixels * sizeof(quint8));
    alpha.read(pixels[3], numberOfPixels * sizeof(quint8));
    
    device->writePlanarBytes(pixels, 0, 0, width, height);
}