/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSTENCILSHAPEFACTORY_H
#define KOSTENCILSHAPEFACTORY_H

#include <KoDocumentResourceManager.h>
#include <KoShapeFactoryBase.h>

class KoProperties;
class KoStore;
class QIODevice;

class StencilShapeFactory : public KoShapeFactoryBase
{
public:
    /// id is the absolute file path
    StencilShapeFactory(const QString &id, const QString &name, const KoProperties *props);
    ~StencilShapeFactory() override;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = new KoDocumentResourceManager()) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

private:
    KoShape *createFromOdf(KoStore *store, KoDocumentResourceManager *documentRes) const;
    KoShape *createFromSvg(QIODevice *in, KoDocumentResourceManager *documentRes) const;
    const KoProperties *m_properties;
};

#endif // KOSTENCILSHAPEFACTORY_H
