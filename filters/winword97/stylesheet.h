#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <myfile.h>

class StyleSheet {

public:
    StyleSheet(const myFile &m);
    ~StyleSheet();

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    void align2(long &adr);

    myFile main;
};
#endif // STYLESHEET_H
