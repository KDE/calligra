// FTXBXS, TeXtBoX Stroy

#ifndef FTXBXS_H
#define FTXBXS_H

struct FTXBXS {
    long cTxbx;      // also iNextReuse
    long cReusable;
    short fReusable;
    long reserved;
    long lid;
    long txidUndo;
};
#endif // FTXBXS_H
