// FLD, Field Descriptor

#ifndef FLD_H
#define FLD_H

struct FLD {
    char ch:5;
    char reserved:3;
    char flt;
    char fDiffer:1;
    char fZombieEmbed:1;
    char fResultDirty:1;
    char fResultEdited:1;
    char fLocked:1;
    char fPrivateResult:1;
    char fNested:1;
    char fHasSep:1;
};
#endif // FLD_H
