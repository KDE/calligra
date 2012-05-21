/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KARBON_PART_H
#define KARBON_PART_H

#include <KoPart.h>

#include "karbonui_export.h"

class QGraphicsItem;
class KoView;
class KoDocument;

class KarbonCanvas;

class KARBONUI_EXPORT KarbonPart : public KoPart
{
    Q_OBJECT

public:
    KarbonPart(QObject *parent);

    virtual ~KarbonPart();

    void setDocument(KoDocument *document);

    /// reimplemented
    virtual KoView *createViewInstance(QWidget *parent);

protected slots:

    /// reimplemented
    virtual void openTemplate(const KUrl& url);

private slots:

    void applyCanvasConfiguration(KarbonCanvas *canvas);
};

#endif
