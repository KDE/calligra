// DTTM, Date and Time (internal date format)

#ifndef DTTM_H
#define DTTM_H

struct  {
    short mint:6;
    short hr:5;
    short dom:5;
    short mon:4;
    short yr:9;
    short wdy:3;
};
#endif // DTTM_H
