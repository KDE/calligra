/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CONNECTIONSHAPEFACTORY_H
#define CONNECTIONSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class KoConnectionShapeFactory : public KoShapeFactoryBase
{
Q_OBJECT
public:
    KoConnectionShapeFactory();
    ~KoConnectionShapeFactory() override {}

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
    QList<KoShapeConfigWidgetBase*> createShapeOptionPanels() override;
};

#endif
