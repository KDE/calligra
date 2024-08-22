/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    ~CalloutShapeFactory() override = default;
    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = nullptr) const override;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    typedef QMap<QString, QVariant> ComplexType;
    typedef QList<QVariant> ListType;

private:
    void addCallout();
    void addRoundedCallout();

    KoProperties *dataToProperties(const QStringList &commands, const ListType &handles, const ComplexType &formulae) const;
    KoProperties *squareProperties() const;
};

#endif // CALLOUTSHAPEFACTORY_H
