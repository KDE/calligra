/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef MOCKTEXTSHAPE_H
#define MOCKTEXTSHAPE_H

#include "../TextShape.h"
#include <KoTextDocumentLayout.h>

class MockTextShape : public TextShape
{
public:
    MockTextShape()
        : TextShape(0)
    {
        layout = qobject_cast<KoTextDocumentLayout *>(textShapeData()->document()->documentLayout());
    }
    void paint(QPainter &painter, const KoViewConverter &converter)
    {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
    }
    virtual void saveOdf(KoShapeSavingContext &) const
    {
    }
    virtual bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &)
    {
        return true;
    }
    KoTextDocumentLayout *layout;
};

#endif
