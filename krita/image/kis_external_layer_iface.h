/*
 *  Copyright (c) 2006 Bart Coppens <kde@bartcoppens.be>
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
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
#ifndef KIS_EXTERNAL_LAYER_IFACE_
#define KIS_EXTERNAL_LAYER_IFACE_

#include "kicon.h"

#include "kis_types.h"

#include "kis_image.h"
#include "kis_layer.h"

class QString;
class QIcon;
class QDomDocument;
class QDomElement;
class KUndo2Command;

/**
   A base interface for layers that are implemented outside the Krita
   core.
 */
class KisExternalLayer : public KisLayer
{

public:
    KisExternalLayer(KisImageWSP image, const QString &name, quint8 opacity)
            : KisLayer(image, name, opacity) {}

    virtual QIcon icon() const {
        return KIcon("system-run");
    }

    virtual void resetCache() {
    }

    virtual KUndo2Command* crop(const QRect & rect) {
        Q_UNUSED(rect);
        return 0;
    }

    virtual KUndo2Command* transform(const QTransform &transform) {
        Q_UNUSED(transform);
        return 0;
    }

};

#endif // KIS_EXTERNAL_IFACE_LAYER_IFACE_
