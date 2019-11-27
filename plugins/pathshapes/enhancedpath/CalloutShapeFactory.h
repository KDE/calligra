/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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

#ifndef CALLOUTSHAPEFACTORY_H
#define CALLOUTSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

/// Factory for callout shapes
class CalloutShapeFactory : public KoShapeFactoryBase
{
public:
    CalloutShapeFactory();
    ~CalloutShapeFactory() override {}
    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = 0) const override;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    typedef QMap<QString, QVariant > ComplexType;
    typedef QList<QVariant> ListType;

private:
    void addCallout();
    void addRoundedCallout();

    KoProperties* dataToProperties(const QStringList &commands, const ListType &handles, const ComplexType &formulae) const;
    KoProperties *squareProperties() const;
};

#endif // CALLOUTSHAPEFACTORY_H
