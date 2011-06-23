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
    KoColorSpace *colorSpace = device->colorSpace();
  
    /* GET KERNEL CODE */
    QVariant text;
    config->getProperty("kernel", text);
    QString kernelCode = text.toString();
    
    if(kernelCode.isEmpty() || kernelCode.size() <= 20) {
      return;
    }
    
    /* CREATE A KERNEL */
    QCLContext context;
    if(!context.create(QCLDevice::GPU)) {
      qFatal("Could not create OpenCL context");
    }
    
    QCLProgram program = context.buildProgramFromSourceCode(qPrintable(kernelCode));
    QCLKernel kritaKernel = program.createKernel("kritaKernel");
    kritaKernel.setGlobalWorkSize(rect.size);
    
    /* CREATE DATA STRUCTURES FOR KERNEL */
    // input
    QCLVector<double> blue  = context.createVector<double>(rect.size);
    QCLVector<double> green = context.createVector<double>(rect.size);
    QCLVector<double> red   = context.createVector<double>(rect.size);
    QCLVector<double> alpha = context.createVector<double>(rect.size);
    
    // output
    QCLVector<double> blue_output  = context.createVector<double>(rect.size);
    QCLVector<double> green_output = context.createVector<double>(rect.size);
    QCLVector<double> red_output   = context.createVector<double>(rect.size);
    QCLVector<double> alpha_output = context.createVector<double>(rect.size);
    
    /* SUPPLY THE DATA TO KERNEL */
    // MathToolbox: handles 8, 16, 32 bits
    QVector<PtrToDouble> toDoubleFuncPtr(device->colorSpace()->channels().count());
    KisMathToolbox* mathToolbox = KisMathToolboxRegistry::instance()->value(device->colorSpace()->mathToolboxId().id());
    if (!mathToolbox->getToDoubleChannelPtr(device->colorSpace()->channels(), toDoubleFuncPtr)) {
      return;
    }
    
    // Iterator
    KisRectIteratorSP iterator = device->createRectIteratorNG(rect);
    int currentPixel = 0;
    do {
      const quint8* pixel = iterator->rawData();
      
      blue[currentPixel]  = toDoubleFuncPtr[0](data, device->colorSpace()->channels()[ki]->pos());
      green[currentPixel] = toDoubleFuncPtr[1](data, device->colorSpace()->channels()[ki]->pos());
      red[currentPixel]   = toDoubleFuncPtr[2](data, device->colorSpace()->channels()[ki]->pos());
      alpha[currentPixel] = toDoubleFuncPtr[3](data, device->colorSpace()->channels()[ki]->pos());
      
      currentPixel+1;
    } while (iterator->nextPixel());
    
    /* APPLY KERNEL */
    kritaKernel(red, red_output);
    
    /* WRITE THE RESULT*/
    // ??
}