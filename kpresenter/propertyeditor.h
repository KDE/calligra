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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qtabdialog.h>
#include <generalproperty.h>

class KCommand;
class KPObject;
class KPrPage;
class KPresenterDoc;
class PenStyleWidget;
class BrushProperty;
class RectProperty;
class PolygonProperty;
class TextProperty;
class KPObjectProperties;

class PropertyEditor : public QTabDialog
{
    Q_OBJECT

public:
    PropertyEditor( QWidget *parent = 0, const char *name = 0, KPrPage *page = 0, KPresenterDoc *doc = 0 );
    ~PropertyEditor();

    KCommand * getCommand();

private slots:
    void slotDone();

private:
    void setupTabs();
    void setupTabPen( bool configureLineEnds );
    void setupTabBrush();
    void setupTabRect();
    void setupTabPolygon();
    void setupTabText();
    void setupTabGeneral();

    GeneralProperty::GeneralValue getGeneralValue();

    KPrPage *m_page;
    KPresenterDoc *m_doc;
    QPtrList<KPObject> m_objects;

    PenStyleWidget *m_penProperty;
    BrushProperty *m_brushProperty;
    RectProperty *m_rectProperty;
    PolygonProperty *m_polygonProperty;
    TextProperty *m_textProperty;
    GeneralProperty *m_generalProperty;

    KPObjectProperties *m_objectProperties;

signals:
    void propertiesOk();
};

#endif /* PROPERTYEDITOR_H */
