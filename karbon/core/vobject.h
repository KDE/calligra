#ifndef __VOBJECT_H__
#define __VOBJECT_H__

class VPainter;

class VObject {
public:
    VObject();

    virtual void draw( VPainter& p ) = 0;

    bool isDirty() { return m_isDirty; }

private:
    bool m_isDirty;	// has this object to be redrawn ?
};

#endif
