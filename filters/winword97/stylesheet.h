#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <myfile.h>
#include <misc.h>
#include <fib.h>
#include <kdebug.h>

struct STYLE {
    bool paragStyle;         // is it a paragraph style?
    unsigned long fcPAPX;    // position of the PAPX grpprl in the table stream if there is one
    unsigned short lenPAPX;  // length of the grpprl
    unsigned long fcCHPX;    // CHPX position
    unsigned short lenCHPX;  // length
};

struct STD {
    unsigned short istdBase;
    QString name;
    STYLE style;
};

class StyleSheet {

public:
    StyleSheet(const myFile &t, const FIB * const f);
    ~StyleSheet();

    const QValueList<STYLE> chain(const unsigned short &sti);    // returns a list with all the PAPX/CHPX/... which
                                                                 // have to be applied (one after the other!)
    const QString styleName(const unsigned short &sti);          // returns the name of a specific style

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    const bool chain_rec(const unsigned short &sti);             // recursively walks back to the NULL-Style

    myFile table;
    const FIB * const fib;
    QMap<unsigned long, STD> styleMap;
    QValueList<STYLE> _chain;                                    // the list with all the styles
    unsigned short lastSti;                                      // don't calculate the chain again if the
                                                                 // list is still correct!
    unsigned short stdOffset;                                    // start of the STD array in the table stream
    unsigned short cstd;                                         // count of styles stored
    unsigned short stdBaseLength;                                // length of the "static" STD part
};
#endif // STYLESHEET_H
