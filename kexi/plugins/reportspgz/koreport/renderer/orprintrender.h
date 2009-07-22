/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef __ORPRINTRENDER_H__
#define __ORPRINTRENDER_H__

#include <QPrinter>
#include <QPainter>
#include "koreport_export.h"

class ORODocument;

class KOREPORT_EXPORT ORPrintRender
{
public:
    ORPrintRender();
    virtual ~ORPrintRender();

    void setPrinter(QPrinter *);
    QPrinter * printer() {
        return m_printer;
    }

    void setPainter(QPainter *);
    QPainter * painter() {
        return m_painter;
    }

    bool setupPrinter(ORODocument *, QPrinter *);
    bool render(ORODocument *);

protected:
    QPrinter* m_printer;
    QPainter* m_painter;
};

#endif // __ORPRINTRENDER_H__



