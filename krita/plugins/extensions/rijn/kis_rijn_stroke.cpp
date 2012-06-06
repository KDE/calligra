/*
 * Copyright (c) 2012 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_rijn_stroke.h"
#include "kis_rijn_canvas.h"
#include "kis_rijn_palette.h"
#include <kis_gtl_lock.h>
#include <kis_transaction.h>
#include <kis_painter.h>

#include <GTLCore/String.h>
#include <GTLCore/CompilationMessages.h>

#include <QDebug>

KisRijnStroke::KisRijnStroke(KisImageWSP _image, KisNodeSP _node, const OpenRijn::Source& _source)
        : KisSimpleStrokeStrategy("rijn", _source.name().c_str()),
        m_image(_image),
        m_node(_node),
        m_source(_source)
{
    enableJob(KisSimpleStrokeStrategy::JOB_INIT);
    enableJob(KisSimpleStrokeStrategy::JOB_FINISH);
    enableJob(KisSimpleStrokeStrategy::JOB_DOSTROKE);
}

void KisRijnStroke::initStrokeCallback()
{
    dbgPlugins << "Init " << m_source.name().c_str();
    KisGtlLocker l;
    m_sketch.setSource(m_source);
    m_sketch.compile();
    if(!m_sketch.isCompiled())
    {
        dbgPlugins << "Compilation of " << m_source.name().c_str() << " has failed.";
        dbgPlugins << m_sketch.compilationMessages().toString().c_str();
    } else {
        dbgPlugins << "Compilation of " << m_source.name().c_str() << " has succeeded.";
    }
    
    m_transaction = new KisTransaction(name(), m_node->paintDevice());
}

void KisRijnStroke::doStrokeCallback(KisStrokeJobData* _data)
{
    Q_UNUSED(_data);
    if(!m_sketch.isCompiled()) return;
    dbgPlugins << "Run " << m_source.name().c_str();
    
    KisRijnCanvas  canvas(m_image, m_node->paintDevice());
    KisRijnPalette palette;
    
    {
        KisGtlLocker l;
        m_sketch.draw(&canvas, &palette);
    }
}

void KisRijnStroke::finishStrokeCallback()
{
    m_transaction->commit(m_image->postExecutionUndoAdapter());
    delete m_transaction;
}
