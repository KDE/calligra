#include "AddFrameData2Shapes.h"
#include <kpresenter/part/KPrShapeApplicationData.h>
#include <libsvg/SvgAnimationData.h>

AddFrameData2Shapes(KoShape *shape){
this.shape = shape;
}

void addNewFrame(Frame *frame) {
  SvgAnimationData * applicationData = dynamic_cast<SvgAnimationData*>( this.shape->applicationData() );

  if ( applicationData == 0 ) {
        applicationData = new SvgAnimationData();
  }

applicationData.setFrame(frame);
  this.shape.setApplicationData(applicationData);
  //'frame' saved as a part of 'shape' now.
  //The SvgWriter will use this 'frame' to extract frame specific attributes.

    }
    //Remove if shapes have default properties. TBC
    void addBasicProperties() {
      
    }
    