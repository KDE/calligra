#ifndef METAEDIT_H
#define METAEDIT_H

#include <qstring.h>

class KMetaEditor
{
public:
    KMetaEditor() {};
    virtual ~KMetaEditor() {};

    virtual bool isOk() { return TRUE; }
    
    virtual QString openFile( const char * ) { return QString(); }
    virtual bool updateFromFile( const char * ) { return false; }
    virtual bool switchToBuffer( const char * ) { return false; }
    virtual bool killBuffer( const char * ) { return false; }
    virtual bool saveBuffer( const char * ) { return false; }
    virtual bool hide() { return false; }
    virtual bool show() { return false; }
};

KMetaEditor* createEditor();

#endif



















