// WKB, WorK Book

#ifndef WKB_H
#define WKB_H

struct WKB {
    short fn;
    unsigned short grfwkb;
    short lvl;
    short fnpt:4;
    short fnpd:12;
    long doc;
};
#endif // WKB_H
