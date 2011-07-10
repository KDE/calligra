#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoParameterShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class KoParameterShape;

class PresentationViewPortShape : public KoParameterShape
{
public:
    PresentationViewPortShape();
    ~PresentationViewPortShape();
  
    /// reimplemented
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    /// reimplemented
    virtual void saveOdf(KoShapeSavingContext &context) const;

    /// reimplemented
    virtual QString pathShapeId() const;

    //Properties of the Rectanglular viewport
    
    /// Returns the corner radius in x-direction
    qreal cornerRadiusX() const;

    /**
     * Sets the corner radius in x-direction.
     *
     * The corner x-radius is a percent value (a number between 0 and 100)
     * of the half size of the rectangles width.
     *
     * @param radius the new corner radius in x-direction
     */
    void setCornerRadiusX(qreal radius);

    /// Returns the corner radius in y-direction
    qreal cornerRadiusY() const;

    /**
     * Sets the corner radius in y-direction.
     *
     * The corner y-radius is a percent value (a number between 0 and 100)
     * of the half size of the rectangles height.
     *
     * @param radius the new corner radius in y-direction
     */
    void setCornerRadiusY(qreal radius);

protected:

    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void updatePath(const QSizeF &size);
    void createPoints(int requiredPointCount);
    void updateHandles();

private:
    qreal m_cornerRadiusX; ///< in percent of half of the rectangle width (a number between 0 and 100)
    qreal m_cornerRadiusY; ///< in percent of half of the rectangle height (a number between 0 and 100)

      
  
};
#endif