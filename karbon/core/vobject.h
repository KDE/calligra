#ifndef __VOBJECT_H__
#define __VOBJECT_H__

class VObject {
public:
    VObject();

    bool isDirty() { return m_isDirty; }

private:
    bool m_isDirty;	// has this object to be redrawn ?
};

#endif
