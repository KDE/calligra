#ifndef __VDRAWABLE_H__
#define __VDRAWABLE_H__

#include "vobject.h"

class QPainter;

class VDrawable : public VObject {
public:
    VDrawable();

    virtual void draw( QPainter& p ) = 0;

protected:    
    
private:

};

#endif
