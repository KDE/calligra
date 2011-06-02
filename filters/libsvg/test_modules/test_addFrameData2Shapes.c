#include "AddFrameData2Shapes.h"
#include <libsvg/Frame.h>
#include <karbon/svg/SvgWriter.h>

int main() {
  KoShape shape = new KoShape();
  Frame frame = new Frame(); //Now frame has default values.
  QList <KoShape*> shapeList = new QList<KoShape*>;
  const QSizeF pageSize = new QSizeF();
  shapeList.first(shape);
  
AddFrameData2Shapes obj = new AddFrameData2Shapes(shape);
  
  //obj.addBasicProperties();
  obj.addNewFrame(frame);
  
  SvgWriter writer = new SvgWriter(shapeList, pageSize);
  //continue from here
}