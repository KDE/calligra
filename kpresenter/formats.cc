#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kimgio.h>
#include "formats.h"
#ifdef HAVE_QIMGIO
#include <qimageio.h>
#endif

//static int numFormats= 6;
#ifdef HAVE_QIMGIO
static int numFormats = 4;
#else
static int numFormats = 5;
#endif

static FormatRecord formatlist[]= {
#ifndef HAVE_QIMGIO
    {
        "JPEG",
        FormatRecord::InternalFormat | FormatRecord::ReadFormat,
        "^\377\330\377\340",
        "*.jpeg *.jpg",
        0, 0,
    },
#endif
    {
        "BMP",
        FormatRecord::InternalFormat | FormatRecord::ReadFormat | FormatRecord::WriteFormat,
        0,
        "*.bmp",
        0, 0,
    },
    {   
        "XBM",
        FormatRecord::InternalFormat | FormatRecord::ReadFormat | FormatRecord::WriteFormat,
        0,
        "*.xbm",
        0, 0,
    },
    {
        "XPM",
        FormatRecord::InternalFormat | FormatRecord::ReadFormat | FormatRecord::WriteFormat,
        0,
        "*.xpm",
        0, 0,
    },
    {
        "PNM",
        0,
        "*.pbm *.pgm *.ppm",
        0, 0,
    }
};

FormatManager::FormatManager()
{
    list.setAutoDelete( TRUE );
    init( formatlist );
}

FormatManager::~FormatManager()
{
}

void FormatManager::init( FormatRecord formatlist[] )
{
    int i;
    FormatRecord *rec;

    // Build format list
    for ( i= 0; i < numFormats; i++ ) {
        list.append( &formatlist[ i ] );
        names.append( formatlist[ i ].formatName );
        globAll.append( formatlist[ i ].glob );
        globAll.append( " " );
    };

    // Register them with Qt
    for ( rec= list.first(); rec != NULL; rec= list.next() ) {
        if ( ( rec->flags & FormatRecord::InternalFormat ) == 0L )
            QImageIO::defineIOHandler( rec->formatName, rec->magic,
                                       0,
                                       rec->read_format, rec->write_format );
    }
    // Register the ones implemented by kimgio ( tiff, jpeg, png, ... )
    kimgioRegister();

#ifdef HAVE_QIMGIO
    qInitImageIO();
#endif
}
    
const QStrList *FormatManager::formats( void )
{
    return &names;
}

const char *FormatManager::allImagesGlob( void )
{
    return globAll;
}

const char *FormatManager::glob( const char *format )
{
    FormatRecord *rec;
    QString name( format );
    QString curr;
    bool done= FALSE;

    rec= list.first();
    do {
        curr= rec->formatName;
        if ( curr == name )
            done= TRUE;
        else
            rec= list.next();
    } while ( !done && ( rec != NULL ) );

    if ( done )
        return rec->glob;
    else
        return NULL;
}

