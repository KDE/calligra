#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <qobject.h>
#include <qlist.h>
#include <style.h>

class StyleSheet : public QObject {

    Q_OBJECT

public:
    StyleSheet();
    ~StyleSheet();

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    void align2(long &adr);

    QList<CStyle> cstyleList;
    //QList<PStyle> pstyleList;
};
#endif // STYLESHEET_H
