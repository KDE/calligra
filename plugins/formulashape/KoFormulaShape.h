/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
                  2009 Jeremias Epperlein <jeeree@web.de>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOFORMULASHAPE_H
#define KOFORMULASHAPE_H

#include <KoShape.h>
#define KoFormulaShapeId "FormulaShapeID"
#include <KoFrameShape.h>

class KoStore;
class KoDocumentResourceManager;
class KoOdfLoadingContext;

class BasicElement;
class FormulaRenderer;
class FormulaData;
class FormulaDocument;

/**
 * @short The flake shape for a formula
 *
 * This class is basically the container for the formula and has also methods to paint,
 * load and save the formula. The formulaRenderer instance that is part of this class
 * serves to paint and layout the formula. The actual formula data means the tree of
 * elements is accessible through the root element which is constructed and destroyed
 * by this class that holds a pointer to it. Everything that goes into the area of
 * editing the formula data is implemented in KoFormulaTool respectively FormulaCursor.
 *
 * @author Martin Pfeiffer <hubipete@gmx.net>
 */
class KoFormulaShape : public KoShape, public KoFrameShape
{
public:
    /// The basic constructor
    explicit KoFormulaShape(KoDocumentResourceManager *documentResourceManager);
    // KoFormulaShape();

    /// The basic destructor
    ~KoFormulaShape() override;

    /// inherited from KoShape
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    void updateLayout();

    /// @return The element at the point @p p
    BasicElement *elementAt(const QPointF &p);

    /// Resize the shape.
    void resize(const QSizeF &size);

    /// @return Get the bounding box of the shape.
    //     QRectF boundingRect() const;

    /// @return the data shown by the shape
    FormulaData *formulaData() const;

    /// @return the formularenderer used to paint this shape
    FormulaRenderer *formulaRenderer() const;

    /**
     * Load a shape from odf - reimplemented from KoShape
     * @param context the KoShapeLoadingContext used for loading
     * @param element element which represents the shape in odf
     * @return false if loading failed
     */
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    bool loadOdfEmbedded(const KoXmlElement &mathElement, KoShapeLoadingContext &context);

    /**
     * @brief store the shape data as ODF XML. - reimplemented from KoShape
     * This is the method that will be called when saving a shape as a described in
     * OpenDocument 9.2 Drawing Shapes.
     * @see saveOdfAttributes
     */
    void saveOdf(KoShapeSavingContext &context) const override;

    KoDocumentResourceManager *resourceManager() const;

private:
    bool loadEmbeddedDocument(KoStore *store, const KoXmlElement &objectElement, const KoOdfLoadingContext &odfLoadingContext);

    /// The data this shape displays
    FormulaData *m_formulaData;

    /// The renderer that takes care of painting the shape's formula
    FormulaRenderer *m_formulaRenderer;

    /// True if this formula is inline, i.e. not embedded in a formula document.
    bool m_isInline;

    FormulaDocument *m_document;
    KoDocumentResourceManager *m_resourceManager;
};

#endif // KOFORMULASHAPE_H
