// DCS, Drop Cap Specifier

#ifndef DCS_H
#define DCS_H

struct DCS {
    short fdct:3;
    short count:5;
    short reserved:8;
};
#endif // DCS_H
