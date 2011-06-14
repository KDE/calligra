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
    /* GET KERNEL CODE */
    QVariant text;
    config->getProperty("kernel", text);
    QString kernelCode = text.toString();
    
    if(kernelCode.isEmpty() || kernelCode.size() <= 20) {
      return;
    }
    
    /* CREATE A KERNEL */
    QCLContext context;                                                                 // context
    if(!context.create(QCLDevice::GPU)) {
      qFatal("Could not create OpenCL context");
    }
    
    QCLProgram program = context.buildProgramFromSourceCode(qPrintable(kernelCode));   // program
    QCLKernel kritaKernel = program.createKernel("kritaKernel");
    kritaKernel.setGlobalWorkSize(100);
    
    /* CREATE DATA STRUCTURES FOR KERNEL */
    QCLVector<int> red = context.createVector<int>(100);
    QCLVector<int> red_output = context.createVector<int>(100);
    
    /* SUPPLY THE DATA TO KERNEL */
    KisRectIteratorSP iterator = device->createRectIteratorNG(0, 0, 10, 10);
    int currentPixel = 0;
    do {
      const quint8* pixel = iterator->rawData();
      red[currentPixel] = *pixel;
      currentPixel+1;
    } while (iterator->nextPixel());
    
    /* APPLY KERNEL */
    kritaKernel(red, red_output);
}