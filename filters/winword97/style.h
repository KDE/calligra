#ifndef STYLE_H
#define STYLE_H

#include <qstring.h>
#include <defs.h>     // from the kword-dir!

class CStyle {

public:
    CStyle();
    CStyle(CStyle &rhs);
    ~CStyle() {}

    const QString format();   // get the <FROMAT> stuff
    const bool dirty() const { return formatDirty; }

    const unsigned short styleID() const { return stID; }

private:
    QString theFormat;
    bool formatDirty;
    unsigned short stID;
    bool pStyle;
};

/*
class PStyle {

public:
    Style();
    Style(QString s, unsigned short ID, bool pSt=true);
    Style(const Style &rhs);
    ~Style() {}

    const QString style() const { return theStyle; }
    const unsigned short styleID() const { return stID; }
    const bool paragraphStyle() const { return pStyle; }

private:
    QString theStyle;
    unsigned short stID;
    bool pStyle;
};
*/
#endif // STYLE_H
