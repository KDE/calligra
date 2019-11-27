/* This file is part of the KDE project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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

#ifndef KOSTENCILSHAPEFACTORY_H
#define KOSTENCILSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>
#include <KoDocumentResourceManager.h>

class KoProperties;
class KoStore;
class QIODevice;

class StencilShapeFactory : public KoShapeFactoryBase
{
public:
    /// id is the absolute file path
    StencilShapeFactory(const QString& id, const QString& name, const KoProperties* props);
    ~StencilShapeFactory() override;

    KoShape* createDefaultShape(KoDocumentResourceManager* documentResources = new KoDocumentResourceManager()) const override;
    bool supports(const KoXmlElement& e, KoShapeLoadingContext& context) const override;

private:
    KoShape* createFromOdf(KoStore* store, KoDocumentResourceManager* documentRes) const;
    KoShape* createFromSvg(QIODevice* in, KoDocumentResourceManager* documentRes) const;
    const KoProperties* m_properties;
};

#endif //KOSTENCILSHAPEFACTORY_H
