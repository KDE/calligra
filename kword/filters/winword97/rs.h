// RS, Routing Slip

#ifndef RS_H
#define RS_H

struct RS {
    short fRouted;
    short fReturnOrig;
    short fTrackStatus;
    short fDirty;
    short nProtect;
    short iStage;
    short delOption;
    short cRecip;
};
#endif // RS_H
