#ifndef FORMATS_H
#define FORMATS_H

#include <qimage.h>
#include <qlist.h>
#include <qstringlist.h>

// This file defines the format records for the supported file formats
// and the manager to install them

class Format
{
public:
  QString formatName;
  unsigned int flags; // Internal? / Read? / Write?
  QString magic; // NULL for formats kpaint should not register
  QString glob;
  image_io_handler read_format; // NULL for internal formats
  image_io_handler write_format; // NULL for internal formats

  // Constants for flags
  static const uint InternalFormat= 1;
  static const uint ReadFormat= 2;
  static const uint WriteFormat= 4;
};

class FormatManager
{
public:
  FormatManager();
  virtual ~FormatManager();
   
  const QString allImagesGlob();
  const QStringList* formats();
  const QString glob( const QString format);
  // const char* description( const char* format );
  // const unsigned int flags( const char* format );

protected:
  virtual void init( Format formatlist[] );

private:
  QList<Format> list;
  QStringList names;
  QString globAll;
};

#endif // FORMATS_H
