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
#include <QWidget>

#include "kis_widget_view.h"
#include "kis_image_document.h"

#include "kis_image.h"

class KisWidgetView::Private {
public:

    Private()
        : document(0)
        , readOnly(true)
    {}

    QWidget *widget;
    KisImageDocument *document;
    bool readOnly;
};

KisWidgetView::KisWidgetView(KisImageDocument *document)
    : d(new Private)
{
    d->document = document;
    QLabel *label = new QLabel(document->image()->objectName());
    d->widget = label;
}

KisWidgetView::~KisWidgetView()
{
    delete d;
}

QString KisWidgetView::title() const
{
    // XXX: Should be the filename?
    if (d->document->image()) {
        return d->document->image()->objectName();
    }
}

bool KisWidgetView::isModifiable() const
{
    return false;
}

bool KisWidgetView::isReadOnly() const
{
    return d->readOnly;
}

void KisWidgetView::setReadOnly( bool isReadOnly )
{
    d->readOnly = isReadOnly;
}

void KisWidgetView::setFocus()
{
    d->widget->setFocus();
}

QWidget* KisWidgetView::widget() const
{
    return d->widget;
}

bool KisWidgetView::hasFocus() const
{
    return d->widget->hasFocus();
}
