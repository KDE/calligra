// DOPTYPOGRAPHY, Document Typography Info

#ifndef DOPTYPOGRAPHY_H
#define DOPTYPOGRAPHY_H

struct DOPTYPOGRAPHY {
    short fKerningPunct:1;
    short iJustification:2;
    short iLevelOfKinsoku:2;
    short f2on1:1;
    short reserved:10;
    short cchFollowingPunct;
    short cchLeadingPunct;
    short rgxchFPunct[101];
    short rgxchLPunct[51];
};
#endif // DOPTYPOGRAPHY_H
