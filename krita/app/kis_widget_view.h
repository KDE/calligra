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
#ifndef KIS_WIDGET_VIEW_H
#define KIS_WIDGET_VIEW_H

#include <abstractwidgetview.h>

class KisImageDocument;

class KisWidgetView : public Kasten2::AbstractWidgetView
{
public:
    KisWidgetView(KisImageDocument *document);
    ~KisWidgetView();

public: // AbstractModel API
    virtual QString title() const;
    virtual bool isModifiable() const;
    virtual bool isReadOnly() const;
    virtual void setReadOnly( bool isReadOnly );

public: // AbstractView API
    virtual void setFocus();
    virtual QWidget* widget() const;
    virtual bool hasFocus() const;

private:
    class Private;
    Private * const d;
};

#endif // KIS_WIDGET_VIEW_H
