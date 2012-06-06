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

#include "kis_rijn_sketch_action.h"

#include <GTLCore/String.h>

#include <QDebug>

KisRijnSketchAction::KisRijnSketchAction(QObject* _parent, KisView2* _view, const OpenRijn::Source& _source) : QObject(_parent), m_view(_view), m_source(_source)
{

}

void KisRijnSketchAction::activated()
{
    qDebug() << "Run " << m_source.name().c_str();
}

#include "kis_rijn_sketch_action.moc"
