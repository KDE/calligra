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

#include <qtabdialog.h>
#include <kspread_generalProperty.h>

class KCommand;

namespace KSpread
{

class EmbeddedObject;
class Sheet;
class Doc;
// class KPrPenStyleWidget;
// class KPrBrushProperty;
// class KPrRectProperty;
// class KPrPolygonProperty;
// class KPrPieProperty;
// class KPrPictureProperty;
// class KPrTextProperty;
// class KPrObjectProperties;

class PropertyEditor : public QTabDialog
{
    Q_OBJECT

public:
    PropertyEditor( QWidget *parent = 0, const char *name = 0, Sheet *page = 0, Doc *doc = 0 );
    ~PropertyEditor();

    KCommand * getCommand();

private slots:
    void slotDone();

private:
     void setupTabs();
//     void setupTabPen( bool configureLineEnds );
//     void setupTabBrush();
//     void setupTabRect();
//     void setupTabPolygon();
//     void setupTabPie();
//     void setupTabPicture();
//     void setupTabText();
    void setupTabGeneral();

    GeneralProperty::GeneralValue getGeneralValue();

    Sheet *m_page;
    Doc *m_doc;
    QPtrList<EmbeddedObject> m_objects;

//     KPrPenStyleWidget *m_penProperty;
//     KPrBrushProperty *m_brushProperty;
//     KPrRectProperty *m_rectProperty;
//     KPrPolygonProperty *m_polygonProperty;
//     KPrPieProperty *m_pieProperty;
//     KPrPictureProperty *m_pictureProperty;
//     KPrTextProperty *m_textProperty;
    GeneralProperty *m_generalProperty;

    //KPrObjectProperties *m_objectProperties;

signals:
    void propertiesOk();
};

}

#endif /* PROPERTYEDITOR_H */
