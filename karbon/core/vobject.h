#ifndef __VOBJECT_H__
#define __VOBJECT_H__

class QPainter;

class VObject {
public:
    VObject();

    virtual void draw( QPainter& p ) = 0;
    
    virtual void translate( double& dx, double& dy ) = 0;

private:

};

#endif
