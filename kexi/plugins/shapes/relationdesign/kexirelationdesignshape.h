/* This file is part of the KDE project
   Copyright (C) 2009-2010 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIRELATIONDESIGNSHAPE_H
#define KEXIRELATIONDESIGNSHAPE_H

#include <KoShape.h>
#include <KoFrameShape.h>
#include "simplefield.h"

class KDbConnection;
class KDbConnectionData;
class KDbTableOrQuerySchema;

#define KEXIRELATIONDEISGNSHAPEID "KexiRelationDesignShape"

class KexiRelationDesignShape : public KoShape, public KoFrameShape
{
public:
    KexiRelationDesignShape();
    ~KexiRelationDesignShape();

    virtual void saveOdf(KoShapeSavingContext &context) const;
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    virtual void paint(QPainter &painter, const KoViewConverter &converter,
                       KoShapePaintingContext &paintContext);
    void constPaint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext) const;

    virtual void setSize(const QSizeF &size);

    void setConnectionData(KDbConnectionData *);
    KDbConnectionData *connectionData();

    void setRelation(const QString &);

    KDbConnection *connection();

protected:
    // reimplemented
    virtual bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context);

private:
    void addConnectionPoints();

    //Data for display
    QString m_relation;
    QString m_database;
    QVector<SimpleField *> m_fieldData;

    //Other data
    KDbConnectionData *m_connectionData;
    KDbConnection *m_connection;
    KDbTableOrQuerySchema *m_relationSchema;

};

#endif // KEXIRELATIONDESIGNSHAPE_H
