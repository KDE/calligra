#ifndef __char_h__
#define __char_h__

#include "format.h"

#include <qimage.h>
#include <qpixmap.h>

enum ClassIDs {ID_KWCharNone = 0,ID_KWCharFormat = 1,ID_KWCharImage = 2};
 
class KWCharAttribute
{
public:
  KWCharAttribute() { classId = ID_KWCharNone; }
  virtual ~KWCharAttribute() {}

  int getClassId()
    { return classId; }

protected:
  int classId;

};

class KWCharFormat : public KWCharAttribute
{
public:
  KWCharFormat() : KWCharAttribute() { classId = ID_KWCharFormat; format = 0L; }
  KWCharFormat( KWFormat* _format ) : KWCharAttribute() { classId = ID_KWCharFormat; format = _format; }
  virtual ~KWCharFormat() { format->decRef(); format = 0L; }

  virtual KWFormat* getFormat()
    { return format; }
  virtual void setFormat(KWFormat *_format)
    { format = _format; }

protected:
  KWFormat *format;

};

class KWCharImage : public KWCharAttribute
{
public:
  KWCharImage() { classId = ID_KWCharImage; }
  virtual ~KWCharImage() {}

  virtual int getClassId()
    { return ID_KWCharImage; }

protected:
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

class KWString
{
public:
  KWString() 
    { _max_ = 0; _len_ = 0; _data_ = 0L; }
  KWString(const char* _text);
  KWString(const KWString &_string);
  ~KWString() 
    { free(_data_,_len_); delete [] _data_; }

  unsigned int size() 
    { return _len_; }
  unsigned int max()
    { return _max_; }
  void append(KWChar *_text,unsigned int _len);
  void insert(unsigned int _pos, const char *_text);
  void insert(unsigned int _pos, const char _c);
  void resize(unsigned int _size,bool del = true);
  bool remove(unsigned int _pos,unsigned int _len = 1);
  KWChar* split(unsigned int _pos);

  KWChar* data()
    { return _data_; }

protected:
  KWChar* alloc(unsigned int _size);
  void free(KWChar* _data,unsigned int _len);
  KWChar* copy(KWChar *_data,unsigned int _len);
  
  unsigned int _len_;
  unsigned int _max_;
  KWChar* _data_;
  
};

void freeChar(KWChar& _char);

#endif
