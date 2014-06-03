/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2012 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBONPRINTJOB_H
#define KARBONPRINTJOB_H

#include <KoPrintingDialog.h>

class KarbonView;

class KarbonPrintJob : public KoPrintingDialog
{
public:
    // the available print modes
    enum PrintMode {
        PrintToPaper, ///< printing to to printer device
        PrintToPdf    ///< printing to pdf file
    };

    KarbonPrintJob(KarbonView *view, PrintMode printMode);

protected:
    virtual QRectF preparePage(int pageNumber);
    virtual QList<KoShape*> shapesOnPage(int pageNumber);
    virtual QList<QWidget*> createOptionWidgets() const;

private:
    KarbonView *m_view;
};

#endif // KARBONPRINTJOB_H
