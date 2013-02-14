/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
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
#include "kis_view_factory.h"
#include "kis_image_document.h"
#include "kis_widget_view.h"

KisViewFactory::KisViewFactory(QObject *parent)
    : Kasten2::AbstractViewFactory(parent)
{
}


virtual Kasten2::AbstractView* createViewFor(Kasten2::AbstractDocument* document)
{
    KisWidgetView *view = 0;
    KisImageDocument *document = qobject_cast<KisImageDocument*>(document);
    if (document) {
        KisWidgetView = new KisWidgetView();
        view->set
    }
    return view;
}
