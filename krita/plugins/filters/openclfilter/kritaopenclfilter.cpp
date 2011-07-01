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
#include <QTimer>
#include <qclcontext.h>
#include <QTime>

KritaOpenCLFilter::KritaOpenCLFilter() : KisFilter(id(), categoryOther(), i18n("&OpenCL..."))
{
    setSupportsPainting(false);
    setSupportsIncrementalPainting(false);
    setSupportsAdjustmentLayers(false);
    setSupportsThreading(true);
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
    int numberOfPixels = rect.height() * rect.width();
  
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
    kritaKernel.setGlobalWorkSize(numberOfPixels);
    
    /* CREATE DATA STRUCTURES FOR KERNEL */
    // Input
    QCLVector<double> blue  = context.createVector<double>(numberOfPixels);
    QCLVector<double> green = context.createVector<double>(numberOfPixels);
    QCLVector<double> red   = context.createVector<double>(numberOfPixels);
    QCLVector<double> alpha = context.createVector<double>(numberOfPixels);
    
    // Output
    QCLVector<double> blue_output  = context.createVector<double>(numberOfPixels);
    QCLVector<double> green_output = context.createVector<double>(numberOfPixels);
    QCLVector<double> red_output   = context.createVector<double>(numberOfPixels);
    QCLVector<double> alpha_output = context.createVector<double>(numberOfPixels);
    
    /* SUPPLY THE DATA TO KERNEL */
    // MathToolbox: change rawData to doubles
    KisMathToolbox* mathToolbox = KisMathToolboxRegistry::instance()->value(device->colorSpace()->mathToolboxId().id());
    QVector<PtrToDouble> toDoubleFuncPtr(device->colorSpace()->channels().count());
    if (!mathToolbox->getToDoubleChannelPtr(device->colorSpace()->channels(), toDoubleFuncPtr)) {
      return;
    }
    
    // Read pixels  
    KisRectIteratorSP readIterator = device->createRectIteratorNG(rect);
    int currentPixel = 0;
    do {
      const quint8* pixel = readIterator->rawData();
      
      blue[currentPixel]  = toDoubleFuncPtr[0](pixel, device->colorSpace()->channels()[0]->pos());
      green[currentPixel] = toDoubleFuncPtr[1](pixel, device->colorSpace()->channels()[1]->pos());
      red[currentPixel]   = toDoubleFuncPtr[2](pixel, device->colorSpace()->channels()[2]->pos());
      alpha[currentPixel] = toDoubleFuncPtr[3](pixel, device->colorSpace()->channels()[3]->pos());
      
      ++currentPixel;
    } while (readIterator->nextPixel());
    
    /* APPLY KERNEL */
    kritaKernel(blue, green, red, alpha, blue_output, green_output, red_output, alpha_output);
    
    /* WRITE THE RESULT*/
    //MathToolbox: change doubles back to rawData
    QVector<PtrFromDouble> fromDoubleFuncPtr(device->colorSpace()->channels().count());
    if (!mathToolbox->getFromDoubleChannelPtr(device->colorSpace()->channels(), fromDoubleFuncPtr)) {
      return;
    }
    
    // Write Pixels
    KisRectIteratorSP writeIterator = device->createRectIteratorNG(rect);
    currentPixel = 0;
    do {
      quint8* pixel = writeIterator->rawData();
      
      fromDoubleFuncPtr[0](pixel, device->colorSpace()->channels()[0]->pos(), blue_output[currentPixel]);
      fromDoubleFuncPtr[1](pixel, device->colorSpace()->channels()[1]->pos(), green_output[currentPixel]);
      fromDoubleFuncPtr[2](pixel, device->colorSpace()->channels()[2]->pos(), red_output[currentPixel]);
      fromDoubleFuncPtr[3](pixel, device->colorSpace()->channels()[3]->pos(), alpha_output[currentPixel]);
      
      ++currentPixel;
    } while (writeIterator->nextPixel());
}