#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <myfile.h>
#include <qvaluelist.h>

class StyleSheet {

public:
    StyleSheet(const myFile &m);
    ~StyleSheet();

    const QValueList<long> chain(const long &styleID);   // returns a list with all the PAPX/CHPX/... which
                                                         // have to be applied (one after the other!)
private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    void chain_rec(const long &styleID);
    void align2(long &adr);

    myFile main;
    QValueList<long> _chain;
};
#endif // STYLESHEET_H
