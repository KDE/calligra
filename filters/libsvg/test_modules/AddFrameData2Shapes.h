#include <libs/flake/KoShape.h>
class AddFrameData2Shapes {
public:
  AddFrameData2Shapes(KoShape *shape);
  ~AddFrameData2Shapes();
  
  /*
 * This will be used when frame properties will be assigned to a shape.
 * @shape The shape to which the properties are bein assigned
 * @frame The Frame object which carries the values of the attributes for this shape.
 */
  void addNewFrame(Frame *frame);
  
  /*
   * Add some arbitrary properties to the shape
   */
  void addBasicProperties();
  
private:
  KoShape *shape;
};