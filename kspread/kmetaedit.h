#ifndef METAEDIT_H
#define METAEDIT_H

#include <qstring.h>

class KMetaEditor
{
public:
    KMetaEditor() {};
    ~KMetaEditor() {};

    virtual bool isOk() { return TRUE; }
    
    virtual QString openFile( const char *_file ) { }
    virtual bool updateFromFile( const char *_file ) { }
    virtual bool switchToBuffer( const char *_buffer ) { }
    virtual bool killBuffer( const char *_buffer ) { }
    virtual bool saveBuffer( const char *_buffer ) { }
    virtual bool hide() { }
    virtual bool show() { }
};

KMetaEditor* createEditor();

#endif



















