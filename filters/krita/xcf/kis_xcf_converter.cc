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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <magick/api.h>

#include <qfile.h>
#include <qstring.h>

#include <kdeversion.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include <qcolor.h>

#include "kis_types.h"
#include "kis_global.h"
#include "kis_doc.h"
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_undo_adapter.h"
#include "kis_xcf_converter.h"
#include "kis_colorspace_registry.h"
#include "kis_iterators_pixel.h"
#include "kis_abstract_colorspace.h"
#include "kis_profile.h"
#include "kis_annotation.h"

#include "../../../config.h"

namespace {

    const PIXELTYPE PIXEL_BLUE = 0;
    const PIXELTYPE PIXEL_GREEN = 1;
    const PIXELTYPE PIXEL_RED = 2;
    const PIXELTYPE PIXEL_ALPHA = 3;

    KisAbstractColorSpace * getColorSpaceForColorType() {
        return KisColorSpaceRegistry::instance() -> get(KisID("RGBA", ""));
    }

    KisProfileSP getProfileForProfileInfo(const Image * image, KisAbstractColorSpace * cs)
    {
    }

    void setAnnotationsForImage(const Image * src, KisImageSP image)
    {
    }

    void exportAnnotationsForImage(Image * dst, vKisAnnotationSP_it& it, vKisAnnotationSP_it& annotationsEnd)
    {
    }

    MagickBooleanType monitor(const char *text, const ExtendedSignedIntegralType, const ExtendedUnsignedIntegralType, ExceptionInfo *)
    {
        KApplication *app = KApplication::kApplication();

        Q_ASSERT(app);

        if (app -> hasPendingEvents())
            app -> processEvents();

        printf("%s\n", text);
        return MagickTrue;
    }
}

KisXCFConverter::KisXCFConverter(KisDoc *doc, KisUndoAdapter *adapter)
{
    m_doc = doc;
    m_adapter = adapter;
    m_job = 0;
    m_stop = false;
}

KisXCFConverter::~KisXCFConverter()
{
}

KisImageBuilder_Result KisXCFConverter::decode(const KURL& uri)
{
    if (m_stop) {
        m_img = 0;
        return KisImageBuilder_RESULT_INTR;
    }

    emit notifyProgressStage(this, i18n("Importing..."), 0);

    m_img = 0;

    // Decode the xcf file



    emit notifyProgressDone(this);

    return KisImageBuilder_RESULT_OK;

}

KisImageBuilder_Result KisXCFConverter::buildImage(const KURL& uri)
{
    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!KIO::NetAccess::exists(uri, false, qApp -> mainWidget())) {
        return KisImageBuilder_RESULT_NOT_EXIST;
    }

    // We're not set up to handle asynchronous loading at the moment.
    KisImageBuilder_Result result = KisImageBuilder_RESULT_FAILURE;

    QString tmpFile;

    if (KIO::NetAccess::download(uri, tmpFile, qApp -> mainWidget())) {
        result = decode(tmpFile);
        KIO::NetAccess::removeTempFile(tmpFile);
    }

    QFile in(tmpFile);
    if(!in.open(IO_ReadOnly)) {
        KMessageBox::sorry( 0L, i18n("Cannot open file - please report.") );
        in.close();
        return KisImageBuilder_RESULT_NOT_EXIST ;
    }

    // Check the header
    char tag[14];
    if (in.readBlock(&tag, 14) != 14) return KisImageBuilder_FAILURE;

    return result;
}


KisImageSP KisXCFConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisXCFConverter::buildFile(const KURL&, KisImageSP image, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd)
{
    return KisImageBuilder_RESULT_UNSUPPORTED;
}


void KisXCFConverter::cancel()
{
    m_stop = true;
}

#include "kis_xcf_converter.moc"

