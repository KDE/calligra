/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COMMENTSHAPEFACTORY_H
#define COMMENTSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

#define COMMENTSHAPEID "CommentShape"

class CommentShapeFactory : public KoShapeFactoryBase
{
public:
    CommentShapeFactory();
    virtual ~CommentShapeFactory();

    virtual bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const;
    virtual KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const;
};

#endif // COMMENTSHAPEFACTORY_H
