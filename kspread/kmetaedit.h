#ifndef METAEDIT_H
#define METAEDIT_H

#include <qstring.h>

class KMetaEditor
{
public:
    KMetaEditor() {};
    ~KMetaEditor() {};

    virtual bool isOk() { return TRUE; }
    
    virtual QString openFile( const char *_file ) { return 0; }
    virtual bool updateFromFile( const char *_file ) { return false; }
    virtual bool switchToBuffer( const char *_buffer ) { return false; }
    virtual bool killBuffer( const char *_buffer ) { return false; }
    virtual bool saveBuffer( const char *_buffer ) { return false; }
    virtual bool hide() { return false; }
    virtual bool show() { return false; }
};

KMetaEditor* createEditor();

#endif



















