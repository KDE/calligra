/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MOCKTEXTSHAPE_H
#define MOCKTEXTSHAPE_H

#include <KoShapeContainer.h>
#include <KoTextDocumentLayout.h>
#include <KoTextShapeData.h>

class MockTextShape : public KoShapeContainer
{
public:
    MockTextShape()
    {
        KoTextShapeData *textShapeData = new KoTextShapeData();
        setUserData(textShapeData);
        layout = new KoTextDocumentLayout(textShapeData->document());
        layout->addShape(this);
        textShapeData->document()->setDocumentLayout(layout);
    }
    virtual void paintComponent(QPainter &, const KoViewConverter &)
    {
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
