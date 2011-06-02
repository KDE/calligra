#include <libs/flake/KoShapeApplicationData.h>
#include "Frame.h"

/*
 * Will extend KoshapeApplicationData
 * To store a Frame object
 */
class SvgAnimationData : public KoShapeApplicationData
{
public:
    SvgAnimationData();
    ~SvgAnimationData();

    /*
     * Sets the frame properties for the calling shape
     */
    void setFrame(Frame *frame);
    /*
     * Returns the Frame object which contains information for the shape animation.
     */
    Frame* getFrame();
   
    private:
    // The Frame object which contains frame attributes and their values
     Frame *frame;
};

