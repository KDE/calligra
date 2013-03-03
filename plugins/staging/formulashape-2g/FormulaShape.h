/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat <gopalakbhat@gmail.com>
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

#ifndef FORMULASHAPE_H
#define FORMULASHAPE_H

// Qt
#include <QObject>

// Calligra
#include <KoFrameShape.h>
#include <KoShape.h>

#define FORMULASHAPEID "FormulaShape2g"

class KoStore;
class KoOdfLoadingContext;
class KoDocumentResourceManager;

class FormulaDocument;

class FormulaShape : public QObject, public KoShape, public KoFrameShape
{
    Q_OBJECT

public:
    FormulaShape(KoDocumentResourceManager *documentResourceManager);
    virtual ~FormulaShape();

    // reimplemented from KoShape
    virtual void paint(QPainter &painter, const KoViewConverter &converter,
                       KoShapePaintingContext &paintcontext);
    // reimplemented from KoShape
    virtual void saveOdf(KoShapeSavingContext &context) const;
    // reimplemented from KoShape
    bool loadOdf( const KoXmlElement& element, KoShapeLoadingContext& context );

    virtual bool loadOdfFrameElement(const KoXmlElement& element, KoShapeLoadingContext& context);

    // reimplemented from KoShape
    virtual void waitUntilReady(const KoViewConverter &converter, bool asynchronous) const;

    KoDocumentResourceManager *resourceManager() const;

    void setMML(const QString &mmlText);
    QString MML();

private:
    bool loadEmbeddedDocument(KoStore *store,const KoXmlElement &objectElement,
                              const KoOdfLoadingContext &odfLoadingContext);

    FormulaDocument *m_document;
    KoDocumentResourceManager *m_resourceManager;

    /// True if this formula is inline, i.e. not embedded in a formula document.
    bool m_isInline;

    QString m_mmlString;
};


#endif
