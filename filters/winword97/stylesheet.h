#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <myfile.h>
#include <qvaluelist.h>

struct STYLE {
    bool paragStyle;     // is it a paragraph style?
    long fc;             // position of grpprl in main stream
};

class StyleSheet {

public:
    StyleSheet(const myFile &m);
    ~StyleSheet();

    const QValueList<STYLE> chain(const unsigned short &sti);  // returns a list with all the PAPX/CHPX/... which
                                                               // have to be applied (one after the other!)
    const QString styleName(const unsigned short &sti);        // returns the name of a specific style

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    bool chain_rec(const unsigned short &sti);                 // recursively walks back to the NULL-Style
    void align2(long &adr);                                    // all blocks start on even addresses!

    myFile main;
    QValueList<STYLE> _chain;                                  // the list with all the styles
    unsigned short lastSti;                                    // don't calculate the chain again if the
                                                               // list is still correct!
};
#endif // STYLESHEET_H
