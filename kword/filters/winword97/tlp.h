// TLP, Table Autoformat Look sPecifier

#ifndef TLP_H
#define TLP_H

struct TLP {
    short itl;
    short fBorders:1;
    short fShading:1;
    short fFont:1;
    short fColor:1;
    short fBestFit:1;
    short fHdrRows:1;
    short fLastRow:1;
    short fHdrCols:1;
    short fLastCol:1;
    short unused:7;
};
#endif // TLP_H
