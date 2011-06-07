#include "SvgWriter.h"
#include "SvgWriter_generic.h"
//#include "SvgAnimationData.h"
//#include "KoShape.h"

SvgWriter::SvgWriter(const QList<KoShapeLayer*> &layers, const QSizeF& pageSize): SvgWriter_generic(layers, pageSize)
{

  
}


SvgWriter::SvgWriter(const QList< KoShape* >& toplevelShapes, const QSizeF& pageSize): SvgWriter_generic(toplevelShapes, pageSize)
{

}



SvgWriter::~SvgWriter()
{

}

void SvgWriter::saveAppData(KoShape *shape)
{
  //No extra app data to be written
}
    

      