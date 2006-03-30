/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <xcfexport.h>

#include <kurl.h>
#include <kgenericfactory.h>

#include <KoDocument.h>
#include <KoFilterChain.h>

#include <kis_doc.h>
#include <kis_image.h>
#include <kis_annotation.h>
#include <kis_types.h>
#include <kis_xcf_converter.h>
//Added by qt3to4:
#include <Q3CString>

typedef KGenericFactory<XCFExport, KoFilter> XCFExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritaxcfexport, XCFExportFactory("kofficefilters"))

XCFExport::XCFExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

XCFExport::~XCFExport()
{
}

KoFilter::ConversionStatus XCFExport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(41008) << "xcf export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KUrl url(filename);

    KisImageSP img = output->currentImage();
    if (!img) return KoFilter::ParsingError;

    KisXCFConverter ib(output, output->undoAdapter());

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    
    if (ib.buildFile(url, img, beginIt, endIt) == KisImageBuilder_RESULT_OK) {
        return KoFilter::OK;
    }

    return KoFilter::InternalError;
}

#include <xcfexport.moc>

