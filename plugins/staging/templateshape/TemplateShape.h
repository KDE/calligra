/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEMPLATESHAPE_H
#define TEMPLATESHAPE_H

// Qt
#include <QObject>

// Calligra
#include <KoShape.h>

// This shape
// #include "Foo.h"

#define TEMPLATESHAPEID "TemplateShape"

class TemplateShape : public QObject, public KoShape
{
    Q_OBJECT

public:
    TemplateShape();
    virtual ~TemplateShape();

    // reimplemented from KoShape
    virtual void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext);
    // reimplemented from KoShape
    virtual void saveOdf(KoShapeSavingContext &context) const;
    // reimplemented from KoShape
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    // reimplemented from KoShape
    virtual void waitUntilReady(const KoViewConverter &converter, bool asynchronous) const;

private:
    // Shape members here.  D-pointer is not needed since this is not a library.
};

#endif
