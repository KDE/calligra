/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ora_converter.h"

#include <kapplication.h>

#include <kio/netaccess.h>
#include <kio/deletejob.h>

#include <KoStore.h>

#include <kis_doc2.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_open_raster_stack_save_visitor.h>
#include <kis_paint_layer.h>
#include <kis_undo_adapter.h>

#include "ora_save_context.h"

OraConverter::OraConverter(KisDoc2 *doc, KisUndoAdapter *adapter)
{
    m_doc = doc;
    m_adapter = adapter;
    m_job = 0;
    m_stop = false;
}

OraConverter::~OraConverter()
{
}

KisImageBuilder_Result OraConverter::decode(const KUrl& uri)
{
    // open the file
#if 0
     FILE *fp = fopen(QFile::encodeName(uri.path()), "rb");
    if (!fp)
    {
        return (KisImageBuilder_RESULT_NOT_EXIST);
    }
    // Creating the KisImageSP
    if( ! m_img) {
        m_img = new KisImage(m_doc->undoAdapter(),  cinfo.image_width,  cinfo.image_height, cs, "built image");
        Q_CHECK_PTR(m_img);
    }
    KisPaintLayerSP layer = new KisPaintLayer(m_img.data(), m_img -> nextLayerName(), quint8_MAX));
#endif

    return KisImageBuilder_RESULT_OK;
}



KisImageBuilder_Result OraConverter::buildImage(const KUrl& uri)
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
        KUrl uriTF;
        uriTF.setPath( tmpFile );
        result = decode(uriTF);
        KIO::NetAccess::removeTempFile(tmpFile);
    }

    return result;
}


KisImageSP OraConverter::image()
{
    return m_img;
}


KisImageBuilder_Result OraConverter::buildFile(const KUrl& uri, KisImageSP image)
{

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (not uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;
    // Open file for writing
    KoStore* store = KoStore::createStore( qApp -> mainWidget(), uri, KoStore::Write, "odr", KoStore::Zip);
    if( not store)
    {
        return KisImageBuilder_RESULT_FAILURE;
    }
    
    OraSaveContext osc( store );
    KisOpenRasterStackSaveVisitor orssv( &osc  );
    
    image->rootLayer()->accept( orssv );
    
    delete store;
    return KisImageBuilder_RESULT_OK;
}


void OraConverter::cancel()
{
    m_stop = true;
}

#include "ora_converter.moc"

