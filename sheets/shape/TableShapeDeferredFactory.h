/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY
#define CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY

#include <KoDeferredShapeFactoryBase.h>
#include <QVariantList>

class KoShapeFactoryBase;

class TableDeferredShapeFactory : public KoDeferredShapeFactoryBase
{
    Q_OBJECT
public:
    TableDeferredShapeFactory(QObject *parent, const QVariantList&);
    ~TableDeferredShapeFactory();

    virtual QString deferredPluginName() { return "spreadsheetshape-deferred"; }

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const;
private:
    KoShapeFactoryBase *m_stubFactory;
};

#endif // CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY
