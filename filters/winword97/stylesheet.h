#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <qobject.h>
#include <qlist.h>
#include <style.h>
#include <myfile.h>

class StyleSheet : public QObject {

    Q_OBJECT

public:
    StyleSheet(const myFile &m);    // creates the 0-Styles
    ~StyleSheet();

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    void align2(long &adr);

    myFile main;
    QList<CStyle> cstyleList;
    QList<PStyle> pstyleList;
};
#endif // STYLESHEET_H
