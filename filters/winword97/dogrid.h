// DOGRID, Drawing Object Grid

#ifndef DOGRID_H
#define DOGRID_H

struct DOGRID {
    short xaGrid;
    short yaGrid;
    short dxaGrid;
    short dyaGrid;
    short dyGridDisplay:7;
    short fTurnItOff:1;
    short dxGridDisplay:7;
    short fFollowMargins:1;
};
#endif // DOGRID_H
