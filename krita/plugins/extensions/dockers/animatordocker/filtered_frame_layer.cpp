/*
 *
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "filtered_frame_layer.h"
#include "animator_manager_factory.h"

#include <kis_adjustment_layer.h>

FilteredFrameLayer::FilteredFrameLayer(const KisGroupLayer& source): SimpleFrameLayer(source)
{
}

FilteredFrameLayer::FilteredFrameLayer(KisImageWSP image, const QString& name, quint8 opacity): SimpleFrameLayer(image, name, opacity)
{
}

FilteredFrameLayer::~FilteredFrameLayer()
{
}

void FilteredFrameLayer::setFilter(KisAdjustmentLayerSP filter)
{
    AnimatorManager *manager = AnimatorManagerFactory::instance()->getManager(image().data());
    manager->setFrameFilter(this, filter.data());
}

KisAdjustmentLayerSP FilteredFrameLayer::filter() const
{
    KisNodeSP node = at(1);
    if (! node || node->name() != "filter")
        return 0;
    KisAdjustmentLayerSP f = qobject_cast<KisAdjustmentLayer*>(node.data());
    return f;
}
