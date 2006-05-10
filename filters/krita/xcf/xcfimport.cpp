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
#include <QString>
//Added by qt3to4:
#include <Q3CString>

#include <xcfimport.h>
#include <kgenericfactory.h>
#include <KoDocument.h>
#include <KoFilterChain.h>

#include <kis_doc.h>
#include <kis_view.h>
#include <kis_xcf_converter.h>
#include <kis_progress_display_interface.h>

typedef KGenericFactory<XCFImport> XCFImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritaxcfimport, XCFImportFactory("kofficefilters"))

XCFImport::XCFImport(KoFilter *, const char *, const QStringList&) : KoFilter(parent)
{
}

XCFImport::~XCFImport()
{
}

KoFilter::ConversionStatus XCFImport::convert(const QByteArray&, const QByteArray& to)
{
    kDebug(41008) << "Importing using XCFImport!\n";

    if (to != "application/x-krita")
        return KoFilter::BadMimeType;

    KisDoc * doc = dynamic_cast<KisDoc*>(m_chain -> outputDocument());

    if (!doc)
        return KoFilter::CreationError;

    KisView * view = 0;

    if (!doc->views().isEmpty()) {
        view = static_cast<KisView*>(doc->views().first());
    }

    QString filename = m_chain -> inputFile();

    doc -> prepareForImport();

    if (!filename.isEmpty()) {

        KUrl url(filename);

        if (url.isEmpty())
            return KoFilter::FileNotFound;

        KisXCFConverter ib(doc, doc -> undoAdapter());

        switch (ib.buildImage(url)) {
            case KisImageBuilder_RESULT_UNSUPPORTED:
                return KoFilter::NotImplemented;
                break;
            case KisImageBuilder_RESULT_INVALID_ARG:
                return KoFilter::BadMimeType;
                break;
            case KisImageBuilder_RESULT_NO_URI:
            case KisImageBuilder_RESULT_NOT_LOCAL:
                return KoFilter::FileNotFound;
                break;
            case KisImageBuilder_RESULT_BAD_FETCH:
            case KisImageBuilder_RESULT_EMPTY:
                return KoFilter::ParsingError;
                break;
            case KisImageBuilder_RESULT_FAILURE:
                return KoFilter::InternalError;
                break;
            case KisImageBuilder_RESULT_OK:
                doc -> setCurrentImage( ib.image());
                return KoFilter::OK;
            default:
                break;
        }

    }
    return KoFilter::StorageCreationError;
}

#include <xcfimport.moc>

