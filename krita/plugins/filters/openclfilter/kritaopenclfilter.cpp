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
    // OpenCL specific
    QCLContext context;
    QCLProgram program;
    QCLKernel kernel;
    QCLImage2D sourceImageBuffer;
    QCLImage2D destinationImageBuffer;
    
    // Image specific
    int width = rect.width();
    int height = rect.height();
    int size = width * height;
    
    // Pixels
   quint8 *inputPixels  = new quint8[size * 4];
   quint8 *outputPixels = new quint8[size * 4];
   device->readBytes(inputPixels, 0, 0, width, height);
    
    // kernel code
    QVariant text;
    config->getProperty("kernel", text);
    QString kernelCode = text.toString();
    
    if(kernelCode.isEmpty()) {
      return;
    }

    /* OPENCL */
    // create context
    if (!context.create(QCLDevice::GPU))
      qFatal("Could not create OpenCL context");

    program = context.buildProgramFromSourceCode(qPrintable(kernelCode));

    // initialize buffers
    QCLImageFormat format(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);
    sourceImageBuffer      = context.createImage2DCopy  (format, inputPixels, QSize(width, height), QCLMemoryObject::ReadOnly);
    destinationImageBuffer = context.createImage2DDevice(format, QSize(width, height), QCLMemoryObject::WriteOnly);

    // kernel
    kernel = program.createKernel("kritaKernel");
    kernel.setGlobalWorkSize(QSize(width, height));
    kernel.setLocalWorkSize(8, 8);

    // run
    kernel(sourceImageBuffer, destinationImageBuffer);
    destinationImageBuffer.read(outputPixels, QRect(0, 0, width, height));
    
    device->writeBytes(outputPixels, 0, 0, width, height);
}