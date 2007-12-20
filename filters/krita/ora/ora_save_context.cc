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

#include "ora_save_context.h"

#include <QDomDocument>

#include <KoStore.h>
#include <KoStoreDevice.h>

#include <kis_paint_layer.h>

#include "kis_png_converter.h"

OraSaveContext::OraSaveContext(KoStore* _store) : m_id(0), m_store(_store)
{
    
}
QString OraSaveContext::saveDeviceData(KisPaintLayerSP layer)
{
    QString filename = QString("data/%1.png").arg( m_id++ );
    if( m_store->open(filename))
    {
        KoStoreDevice io ( m_store );
        KisPNGConverter pngconv(0, layer->image()->undoAdapter());
        vKisAnnotationSP_it annotIt = 0;
        if( pngconv.buildFile(&io, layer->image(), layer->paintDevice(), annotIt, annotIt, 0, false, true) != KisImageBuilder_RESULT_OK)
        {
            kDebug(41008) <<"Saving PNG failed:" << filename;
            return "";
        }
        io.close();
        if(not m_store->close())
        {
            return "";
        }
    } else {
        kDebug(41008) <<"Opening of data file failed :" << filename;
        return "";
    }
    
    return filename;
}
void OraSaveContext::saveStack(const QDomDocument& doc)
{
    if( m_store->open("stack.xml") )
    {
        KoStoreDevice io ( m_store );
        io.write( doc.toByteArray());
    } else {
        kDebug(41008) <<"Opening of the stack.xml file failed :";
    }
}
