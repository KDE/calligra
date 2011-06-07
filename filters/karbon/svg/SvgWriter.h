#include "filters/libsvg/SvgWriter_generic.h"


//class Frame;
//class KoShape;
//class QTextStream;
class SvgWriter;
class KoShapeLayer;

class SvgWriter : public SvgWriter_generic
{
public:
 /// Creates svg writer to export specified layers
      SvgWriter(const QList<KoShapeLayer*> &layers, const QSizeF &pageSize) ;
      
      /// Creates svg writer to export specified shapes
      SvgWriter(const QList<KoShape*> &toplevelShapes, const QSizeF &pageSize);

      /// Destroys the svg writer
      ~SvgWriter();
      
      void saveAppData(KoShape *shape);
 
//private:
      /*
      * Saves the properties associated with the shape used for SVg animation.
      
      void saveFrame(Frame *frame);
      void savePlainText();
      void saveScript();
      void forTesting(KoShape *shape);

  QTextStream* m_frames;
  QString m_script;
  */
};  