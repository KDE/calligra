#include <style.h>

CStyle::CStyle(unsigned short styleID, unsigned char id) {

    if(id>0 && id<6)
        _id=id;
    else
        _id=1;

    stID=styleID;
    formatDirty=true;
    _pos=0;
    _len=0;

    // just very basic init! more to come...
    if(_id==1)
        data.text.fontname=0L;
    else if(_id==2)
        data.image.filename=0L;
    else if(_id==4)
        data.variable.fontname=0L;
    else if(_id==5) {
        data.footnote.before=0L;
        data.footnote.after=0L;
        data.footnote.fontname=0L;
        data.footnote.ref=0L;
    }
}

CStyle::CStyle(CStyle &rhs) {

    if(!rhs.dirty()) {
        //theFormat=rhs.format();
        formatDirty=false;
    }
}

CStyle::~CStyle() {

    if(_id==1) {
        if(data.text.fontname) {
            delete [] data.text.fontname;
            data.text.fontname=0L;
        }
    }
    else if(_id==2) {
        if(data.image.filename) {
            delete [] data.image.filename;
            data.image.filename=0L;
        }
    }
    else if(_id==4) {
        if(data.variable.fontname) {
            delete [] data.variable.fontname;
            data.variable.fontname=0L;
        }
    }
    else if(_id==5) {
        if(data.footnote.fontname) {
            delete [] data.footnote.fontname;
            data.footnote.fontname=0L;
        }
        if(data.footnote.before) {
            delete [] data.footnote.before;
            data.footnote.before=0L;
        }
        if(data.footnote.after) {
            delete [] data.footnote.after;
            data.footnote.after=0L;
        }
        if(data.footnote.ref) {
            delete [] data.footnote.ref;
            data.footnote.ref=0L;
        }
    }
}

void CStyle::textFontname(const QString name) {
    if(_id==1) {
        int len=name.length();
        data.text.fontname=new char[len+1];
        strncpy(data.text.fontname, (const char*)name, len);
        data.text.fontname[len]='\0';
    }
}

void CStyle::imageFilename(const QString name) {
    if(_id==2) {
        int len=name.length();
        data.image.filename=new char[len+1];
        strncpy(data.image.filename, (const char*)name, len);
        data.image.filename[len]='\0';
    }
}
