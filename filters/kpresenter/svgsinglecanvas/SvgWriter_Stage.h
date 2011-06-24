#include "SvgWriter_generic.h"


class Frame;
class KoShape;
class QTextStream;
class SvgWriter;
class KoShapeLayer;
class SvgAnimationData;

class SvgWriter_Stage : public SvgWriter_generic
{
public:
 /// Creates svg writer to export specified layers
      SvgWriter_Stage(const QList<KoShapeLayer*> &layers, const QSizeF &pageSize) ;
      
      /// Creates svg writer to export specified shapes
      SvgWriter_Stage(const QList<KoShape*> &toplevelShapes, const QSizeF &pageSize);

      /// Destroys the svg writer
      ~SvgWriter_Stage();
      
      void saveAppData(KoShape *shape);
 
private:
      
      void savePlainText();
      void saveScript();
      void forTesting(KoShape *shape);

  QTextStream* m_frames;
  QString m_script;
};  