#include <kimgio.h>
#include "formats.h"

//static int numFormats= 6;
static int numFormats= 5;

static Format formatlist[] =
{
  {
    "JPEG",
    Format::InternalFormat | Format::ReadFormat,
    "^\377\330\377\340", "*.jpeg *.jpg", 0, 0,
  },
  {
    "BMP",
    Format::InternalFormat | Format::ReadFormat | Format::WriteFormat,
    0, "*.bmp", 0, 0,
  },
  {	 
    "XBM",
    Format::InternalFormat | Format::ReadFormat | Format::WriteFormat,
    0, "*.xbm", 0, 0,
  },
  {
    "XPM",
    Format::InternalFormat | Format::ReadFormat | Format::WriteFormat,
    0, "*.xpm", 0, 0,
  },
  {
    "PNM",
    Format::InternalFormat | Format::ReadFormat | Format::WriteFormat,
    0, "*.pbm *.pgm *.ppm", 0, 0
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

void FormatManager::init( Format formatlist[] )
{
  int i;
  Format* rec;
   
  // Build format list
  for( i = 0; i < numFormats; i++ )
  {
    list.append( &formatlist[ i ] );
    names.append( formatlist[ i ].formatName );
    globAll.append( formatlist[ i ].glob );
    globAll.append( " " );
  };
   
  // Register them with Qt
  for( rec = list.first(); rec != NULL; rec = list.next() )
  {
    if( ( rec->flags & Format::InternalFormat ) == 0L )
    {
      QImageIO::defineIOHandler(rec->formatName, rec->magic, 0, rec->read_format, rec->write_format );
    }
  }
  // Register the ones implemented by kimgio (tiff, jpeg, png, ...)
  kimgioRegister();
}
	  
const QStringList* FormatManager::formats()
{
  return &names;
}

const QString FormatManager::allImagesGlob()
{
  return globAll;
}
 
const QString FormatManager::glob( const QString format )
{
  Format* rec;
  QString name( format );
  QString curr;
  bool done = FALSE;

  rec = list.first();
  do
  { 
    curr = rec->formatName;
    if( curr == name )
      done = TRUE;
    else 
      rec = list.next();
  }
  while( !done && ( rec != NULL ) );

  if( done )
    return rec->glob;
  else
    return NULL;
}
