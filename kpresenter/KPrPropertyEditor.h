// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <q3tabdialog.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <KPrGeneralProperty.h>

class KCommand;
class KPrObject;
class KPrPage;
class KPrDocument;
class KPrPenStyleWidget;
class KPrBrushProperty;
class KPrRectProperty;
class KPrPolygonProperty;
class KPrPieProperty;
class KPrPictureProperty;
class KPrTextProperty;
class KPrObjectProperties;

class KPrPropertyEditor : public Q3TabDialog
{
    Q_OBJECT

public:
    KPrPropertyEditor( QWidget *parent = 0, const char *name = 0, KPrPage *page = 0, KPrDocument *doc = 0 );
    ~KPrPropertyEditor();

    KCommand * getCommand();

private slots:
    void slotDone();

private:
    void setupTabs();
    void setupTabPen( bool configureLineEnds );
    void setupTabBrush();
    void setupTabRect();
    void setupTabPolygon();
    void setupTabPie();
    void setupTabPicture();
    void setupTabText();
    void setupTabGeneral();

    KPrGeneralProperty::GeneralValue getGeneralValue();

    KPrPage *m_page;
    KPrDocument *m_doc;
    Q3PtrList<KPrObject> m_objects;

    KPrPenStyleWidget *m_penProperty;
    KPrBrushProperty *m_brushProperty;
    KPrRectProperty *m_rectProperty;
    KPrPolygonProperty *m_polygonProperty;
    KPrPieProperty *m_pieProperty;
    KPrPictureProperty *m_pictureProperty;
    KPrTextProperty *m_textProperty;
    KPrGeneralProperty *m_generalProperty;

    KPrObjectProperties *m_objectProperties;

signals:
    void propertiesOk();
};

#endif /* PROPERTYEDITOR_H */
