/*
 *  kis_tool_moutline.h -- part of Krita
 *
 *  Copyright (c) 2006 Emanuele Tamponi <emanuele@valinor.it>
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

#ifndef KIS_TOOL_MOUTLINE_H_
#define KIS_TOOL_MOUTLINE_H_

#include "kis_tool_factory.h"
#include "kis_curve_framework.h"
#include "kis_tool_curve.h"

class KisToolMagnetic;

typedef QValueList< QValueList <bool> > BoolMatrix;

class KisCurveMagnetic : public KisCurve {

    typedef KisCurve super;

    KisToolMagnetic *m_parent;

    void prepareRow (KisPaintDeviceSP src, Q_UINT8* data, Q_UINT32 x, Q_UINT32 y, Q_UINT32 w, Q_UINT32 h);
    BoolMatrix sobel (const QRect & rc, KisPaintDeviceSP src);

public:

    KisCurveMagnetic (KisToolMagnetic *parent);
    ~KisCurveMagnetic ();

    virtual void calculateCurve (iterator, iterator, iterator);

};

class KisToolMagnetic : public KisToolCurve {

    typedef KisToolCurve super;
    Q_OBJECT

    friend class KisCurveMagnetic;

public:

    KisToolMagnetic();
    ~KisToolMagnetic();

    virtual void setup (KActionCollection*);
    virtual enumToolType toolType() { return TOOL_SELECT; }

private:

    KisCurveMagnetic *m_derived;

};

class KisToolMagneticFactory : public KisToolFactory {
    typedef KisToolFactory super;
public:
    KisToolMagneticFactory() : super() {};
    virtual ~KisToolMagneticFactory(){};

    virtual KisTool * createTool(KActionCollection * ac) {
        KisTool * t =  new KisToolMagnetic();
        Q_CHECK_PTR(t);
        t->setup(ac);
        return t;
    }
    virtual KisID id() { return KisID("magneticoutline", i18n("Magnetic Outline Selection Tool")); }
};

#endif // KIS_TOOL_MOUTLINE_H_
