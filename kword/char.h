#ifndef __char_h__
#define __char_h__

#include "format.h"

#include <qimage.h>
#include <qpixmap.h>

#define ID_KWCharFormat 1
#define ID_KWCharImage 2
 
struct KWCharAttribute
{
  int classId;
};

struct KWCharFormat
{
  KWCharFormat() { type.classId = ID_KWCharFormat; }
  KWCharFormat( const KWFormat& _format ) { type.classId = ID_KWCharFormat; format = _format; }

  KWCharAttribute type;
  KWFormat format;
};

struct KWCharImage
{
  KWCharImage() { type.classId = ID_KWCharImage; }

  KWCharAttribute type;
  // We need the image because it has full resolution and
  // color depths. The pixmap may be reduced in both aspects
  // depending on the resolution and color depth of the display.
  QImage image;
  QPixmap pixmap;
};

// Be prepared for unicode
#define kwchar char

struct KWChar
{
  kwchar c;
  KWCharAttribute* attrib;
};

struct KWString
{
  KWString() { max = 0; len = 0; data = 0L; }
  KWString( const char* _text );
  KWString(const KWString &_string);
  ~KWString() { free( data, len ); delete []data; }

  unsigned int size() { return len; }
  void append(KWChar *_text,unsigned int _len);
  void insert( unsigned int _pos, const char *_text );
  void insert( unsigned int _pos, const char _c );
  void resize( unsigned int _size );
  bool remove( unsigned int _pos,unsigned int _len = 1 );

  // Intern
  KWChar* alloc( unsigned int _size );
  void free( KWChar* _data, unsigned int _len );
  KWChar* copy(KWChar *_data,unsigned int _len);
  
  unsigned int len;
  unsigned int max;
  KWChar* data;
  
};

void freeChar( KWChar& _char );

#endif
