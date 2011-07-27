#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoShapeContainer.h>
#include <qpainterpath.h>
#include <KoShape.h>
#include <KoPathShape.h>

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

    virtual void saveOdf(KoShapeSavingContext &context) const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    virtual QPainterPath outline() const;

    virtual QSizeF size() const;

private:     
//    QString toString();
    void setListOfPoints(QList<QPointF> points);
    
  //  QString m_path;
    

   /**
     * @return a default path in the shape of '[ ]'
     */
     QPainterPath createShapePath(const QSizeF& size);
     
     QVector< QVector< int > > createAdjMatrix();
  //   void setAdjMatrix(QVector< QVector < int > >* matrix);
//     QVector< QVector< int > > adjMatrix(); 
     
     void createListOfPoints(const QSizeF& size);
     
  //   QList< QPointF > listOfPoints();
     
     QList<QPointF> m_pointsOfShape;
    
    int m_noOfPoints;
    QVector< QVector<int> > m_adjMatrix;
       
};
#endif
