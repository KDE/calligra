#ifndef STYLE_H
#define STYLE_H

#include <qstring.h>
#include <defs.h>     // from the kword-dir!

class CStyle {

public:
    CStyle(unsigned short styleID, unsigned char id=1);
    CStyle(CStyle &rhs);
    ~CStyle();

    const QString format();   // get the <FROMAT> stuff
    const bool dirty() const { return formatDirty; }

    const unsigned short styleID() const { return stID; }

    // general stuff
    const unsigned char id() const { return _id; }

    const unsigned long pos() const { return _pos; }
    void pos(const unsigned long p) { _pos=p; }

    const unsigned long len() const { return _len; }
    void len(const unsigned long l) { _len=l; }

    // id=1, "normal" text
    const unsigned char textRed() const { return _id==1 ? data.text.red : 0; }
    const unsigned char textGreen() const { return _id==1 ? data.text.green : 0; }
    const unsigned char textBlue() const { return _id==1 ? data.text.blue : 0; }
    const QString textFontname() const { return _id==1 ? QString(data.text.fontname) : QString(0); }
    const unsigned char textSize() const { return _id==1 ? data.text.size : 0; }
    const unsigned char textWeight() const { return _id==1 ? data.text.weight : 0; }
    const unsigned char textUnderline() const { return _id==1 ? data.text.underline : 0; }
    const unsigned char textVertalign() const { return _id==1 ? data.text.vertalign : 0; }

    void textRed(const unsigned char r) { if(_id==1) data.text.red=r; }
    void textGreen(const unsigned char g) { if(_id==1) data.text.green=g; }
    void textBlue(const unsigned char b) { if(_id==1) data.text.blue=b; }
    void textFontname(const QString name);
    void textSize(const unsigned char s) { if(_id==1 && s<101) data.text.size=s; }     // limit?
    void textWeight(const unsigned char w) { if(_id==1 && w<101) data.text.weight=w; } // limit?
    void textUnderline(const unsigned char u) { if(_id==1 && u<2) data.text.underline=u; }
    void textVertalign(const unsigned char v) { if(_id==1 && v<3) data.text.vertalign=v; }

    // id=2, pictures
    const QString imageFilename() const { return _id==2 ? QString(data.image.filename) : QString(0); }
    void imageFilename(const QString name);

    // id=4, variables
    // id=5, footnotes

private:
    QString theFormat;
    bool formatDirty;
    unsigned short stID;

    // KWord data
    unsigned char _id;   // 1=text, 2=image, 3=tab, 4=variable, 5=footnote
    unsigned long _pos;
    unsigned long _len;

    union {
        struct {
            unsigned char red, green, blue;
            char *fontname;
            unsigned char size, weight;
            unsigned char underline;
            unsigned char vertalign;
        } text;
        struct {
            char *filename;
        } image;
        struct {
            unsigned char type;
            unsigned char frameSet, frame, pageNum;
            unsigned short year;
            unsigned char month, day, fix;
            unsigned char hour, minute, second;
            unsigned short msecond;

            unsigned char red, green, blue;
            char *fontname;
            unsigned char size, weight;
            unsigned char underline;
            unsigned char vertalign;
        } variable;
        struct {
            unsigned short from, to;
            char space;
            unsigned short start, end;
            char *before, *after;
            char *ref;

            unsigned char red, green, blue;
            char *fontname;
            unsigned char size, weight;
            unsigned char underline;
            unsigned char vertalign;
        } footnote;
    } data;
};

// I'll have to change this :)
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
