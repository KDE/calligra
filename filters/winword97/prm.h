// PRM, Property Modifier

#ifndef PRM_H
#define PRM_H

struct PRM1 {     // see page 123
    short fComplex:1;
    short isprm:7;
    short val:8;
};

struct PRM2 {    // see page 124
    short fComplex:1;
    short igrpprl:15;
};
#endif // PRM_H
