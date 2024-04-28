/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCOLLECTIONSHAPEFACTORY_H
#define KOCOLLECTIONSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class CollectionShapeFactory : public KoShapeFactoryBase
{
public:
    CollectionShapeFactory(const QString &id, KoShape *shape);
    ~CollectionShapeFactory() override;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

private:
    KoShape *m_shape;
};

#endif // KOCOLLECTIONSHAPEFACTORY_H
