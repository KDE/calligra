#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoShapeContainer.h>
#include <qpainterpath.h>
#include <KoShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class KoParameterShape;

class PresentationViewPortShape : public KoShape
{
public:
  /**
   * @brief Constructor
   * @brief initializes a basic PresentationViewPortShape shaped like a [ ]
   */
    PresentationViewPortShape();
    ~PresentationViewPortShape();
    
    /** Reimplemented methods */
    
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

    /**
     * @brief Paint the component
     * Implement this method to allow the shape to paint itself, just like the KoShape::paint()
     * method does.
     *
     * @param painter used for painting the shape
     * @param converter to convert between internal and view coordinates.
     * @see applyConversion()
     */
    virtual void paintComponent(QPainter &painter, const KoViewConverter &converter);

    virtual void update() const;

    virtual void saveOdf(KoShapeSavingContext &context) const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    /**Properties of the Rectanglular viewport */
    
    /**
     * @return a default path in the shape of '[ ]'
     */
    QPainterPath createShapePath() const;
    
    /**Others */
    QString pathShapeId() const;
        /// reimplemented
    virtual QPainterPath outline() const;
     virtual QSizeF size() const;
private:
    qreal m_cornerRadiusX; ///< in percent of half of the rectangle width (a number between 0 and 100)
    qreal m_cornerRadiusY; ///< in percent of half of the rectangle height (a number between 0 and 100)

      //QPainterPath viewPortPath;
  
};
#endif