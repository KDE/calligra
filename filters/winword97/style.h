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

    // id=3 - nothing
    // id=4, variables
    const unsigned char variableType() const { return _id==4 ? data.variable.type : 0; }
    const unsigned char variableFrameSet() const { return _id==4 ? data.variable.frameSet : 0; }
    const unsigned char variableFrame() const { return _id==4 ? data.variable.frame : 0; }
    const unsigned char variablePageNum() const { return _id==4 ? data.variable.pageNum : 0; }
    const unsigned short variableYear() const { return _id==4 ? data.variable.year : 0; }
    const unsigned char variableMonth() const { return _id==4 ? data.variable.month : 0; }
    const unsigned char variableDay() const { return _id==4 ? data.variable.day : 0; }
    const unsigned char variableFix() const { return _id==4 ? data.variable.fix : 0; }
    const unsigned char variableHour() const { return _id==4 ? data.variable.hour : 0; }
    const unsigned char variableMinute() const { return _id==4 ? data.variable.minute : 0; }
    const unsigned char variableSecond() const { return _id==4 ? data.variable.second : 0; }
    const unsigned short variableMsecond() const { return _id==4 ? data.variable.msecond : 0; }
    const unsigned char variableRed() const { return _id==4 ? data.variable.red : 0; }
    const unsigned char variableGreen() const { return _id==4 ? data.variable.green : 0; }
    const unsigned char variableBlue() const { return _id==4 ? data.variable.blue : 0; }
    const QString variableFontname() const { return _id==4 ? QString(data.variable.fontname) : QString(0); }
    const unsigned char variableSize() const { return _id==4 ? data.variable.size : 0; }
    const unsigned char variableWeight() const { return _id==4 ? data.variable.weight : 0; }
    const unsigned char variableUnderline() const { return _id==4 ? data.variable.underline : 0; }
    const unsigned char variableVertalign() const { return _id==4 ? data.variable.vertalign : 0; }

    void variableType(const unsigned char t) { if(_id==4 && t<6) data.variable.type=t; }
    void variableFrameSet(const unsigned char f) { if(_id==4) data.variable.frameSet=f; }
    void variableFrame(const unsigned char f) { if(_id==4) data.variable.frame=f; }
    void variablePageNum(const unsigned char p) { if(_id==4) data.variable.pageNum=p; }
    void variableYear(const unsigned short y) { if(_id==4) data.variable.year=y; }
    void variableMonth(const unsigned char m) { if(_id==4 && m<13) data.variable.month=m; }
    void variableDay(const unsigned char d) { if(_id==4 && d<32) data.variable.day=d; }
    void variableFix(const unsigned char f) { if(_id==4 && f<2) data.variable.fix=f; }
    void variableHour(const unsigned char h) { if(_id==4 && h<24) data.variable.hour=h; }
    void variableMinute(const unsigned char m) { if(_id==4 && m<60) data.variable.minute=m; }
    void variableSecond(const unsigned char s) { if(_id==4 && s<60) data.variable.second=s; }
    void variableMsecond(const unsigned short m) { if(_id==4 && m<1000) data.variable.msecond=m; }
    void variableRed(const unsigned char r) { if(_id==4) data.variable.red=r; }
    void variableGreen(const unsigned char g) { if(_id==4) data.variable.green=g; }
    void variableBlue(const unsigned char b) { if(_id==4) data.variable.blue=b; }
    void variableFontname(const QString name);
    void variableSize(const unsigned char s) { if(_id==4 && s<101) data.variable.size=s; }     // limit?
    void variableWeight(const unsigned char w) { if(_id==4 && w<101) data.variable.weight=w; } // limit?
    void variableUnderline(const unsigned char u) { if(_id==4 && u<2) data.variable.underline=u; }
    void variableVertalign(const unsigned char v) { if(_id==4 && v<3) data.variable.vertalign=v; }

    // id=5, footnotes
    const unsigned short footnoteFrom() const { return _id==5 ? data.footnote.from : 0; }
    const unsigned short footnoteTo() const { return _id==5 ? data.footnote.to : 0; }
    const char footnoteSpace() const { return _id==5 ? data.footnote.space : 0; }
    const unsigned short footnoteStart() const { return _id==5 ? data.footnote.start : 0; }
    const unsigned short footnoteEnd() const { return _id==5 ? data.footnote.end : 0; }
    const QString footnoteBefore() const { return _id==5 ? QString(data.footnote.before) : QString(0); }
    const QString footnoteAfter() const { return _id==5 ? QString(data.footnote.after) : QString(0); }
    const QString footnoteRef() const { return _id==5 ? QString(data.footnote.ref) : QString(0); }
    const unsigned char footnoteRed() const { return _id==5 ? data.footnote.red : 0; }
    const unsigned char footnoteGreen() const { return _id==5 ? data.footnote.green : 0; }
    const unsigned char footnoteBlue() const { return _id==5 ? data.footnote.blue : 0; }
    const QString footnoteFontname() const { return _id==5 ? QString(data.footnote.fontname) : QString(0); }
    const unsigned char footnoteSize() const { return _id==5 ? data.footnote.size : 0; }
    const unsigned char footnoteWeight() const { return _id==5 ? data.footnote.weight : 0; }
    const unsigned char footnoteUnderline() const { return _id==5 ? data.footnote.underline : 0; }
    const unsigned char footnoteVertalign() const { return _id==5 ? data.footnote.vertalign : 0; }

    void footnoteFrom(const unsigned short f) { if(_id==5) data.footnote.from=f; }
    void footnoteTo(const unsigned short t) { if(_id==5) data.footnote.to=t; }
    void footnoteSpace(const char c) { if(_id==5) data.footnote.space=c; }
    void footnoteStart(const unsigned short s) { if(_id==5) data.footnote.start=s; }
    void footnoteEnd(const unsigned short e) { if(_id==5) data.footnote.end=e; }
    void footnoteBefore(const QString name);
    void footnoteAfter(const QString name);
    void footnoteRef(const QString name);
    void footnoteRed(const unsigned char r) { if(_id==5) data.footnote.red=r; }
    void footnoteGreen(const unsigned char g) { if(_id==5) data.footnote.green=g; }
    void footnoteBlue(const unsigned char b) { if(_id==5) data.footnote.blue=b; }
    void footnoteFontname(const QString name);
    void footnoteSize(const unsigned char s) { if(_id==5 && s<101) data.footnote.size=s; }     // limit?
    void footnoteWeight(const unsigned char w) { if(_id==5 && w<101) data.footnote.weight=w; } // limit?
    void footnoteUnderline(const unsigned char u) { if(_id==5 && u<2) data.footnote.underline=u; }
    void footnoteVertalign(const unsigned char v) { if(_id==5 && v<3) data.footnote.vertalign=v; }

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
