/*
 * Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
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

#ifndef _KIS_RIJN_SKETCH_ACTION_H_
#define _KIS_RIJN_SKETCH_ACTION_H_

#include <QObject>
#include <OpenRijn/Source.h>

namespace OpenRijn {
    class Source;
}

class KisView2;
class KisRijnSketchAction : public QObject
{
    Q_OBJECT
public:
    KisRijnSketchAction(QObject* _parent, KisView2* _view, const OpenRijn::Source& _source);
public slots:
    void activated();
private:
    KisView2* m_view;
    OpenRijn::Source m_source;
};

#endif
