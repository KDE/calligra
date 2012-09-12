/* This file is part of the KDE project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
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
#include <KoProperties.h>

class KoShapeBasedDocumentBase;

class StencilShapeFactory : public KoShapeFactoryBase
{
    public:
        StencilShapeFactory(const QString &id, const QString &name, const QString &source, KoProperties* props);
        ~StencilShapeFactory();

        virtual KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = new KoDocumentResourceManager()) const;
        virtual bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const;

    private:
        KoShape* m_shape;
        KoProperties* m_params;
        QString m_path;
};

#endif //KOSTENCILSHAPEFACTORY_H
