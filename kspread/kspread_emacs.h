#ifndef EMACS_H
#define EMACS_H

class KEmacs;

#include "kmetaedit.h"

class KEmacs : public KMetaEditor
{
public:
    KEmacs();
    virtual ~KEmacs();
    
    virtual bool isOk() { if ( emacsPID > 0 ) return TRUE; return FALSE; }
    
    virtual QString openFile( const char *_file );
    virtual bool updateFromFile( const char *_file );
    virtual bool switchToBuffer( const char *_buffer );
    virtual bool killBuffer( const char *_buffer );
    virtual bool saveBuffer( const char *_buffer );
    virtual bool hide();
    virtual bool show();

protected:
    QString runCmd( const char *_cmd );

    int emacsPID;
};

#endif
