#ifndef __char_h__
#define __char_h__

#include "format.h"
#include "image.h"
#include "searchdia.h"
#include "variable.h"

#include <qimage.h>
#include <qstring.h>
#include <qregexp.h>

#include <iostream>

#include <koStream.h>

class KWordDocument;
class KWTextFrameSet;

enum ClassIDs {ID_KWCharNone = 0,ID_KWCharFormat = 1,ID_KWCharImage = 2,ID_KWCharTab = 3,ID_KWCharVariable = 4};

class KWCharAttribute
{
public:
  KWCharAttribute() { classId = ID_KWCharNone; }
  virtual ~KWCharAttribute() {}

  int getClassId()
    { return classId; }
  virtual bool operator==(const KWCharAttribute &_attrib)
    { return classId == const_cast<KWCharAttribute>(_attrib).getClassId(); }

  virtual void save(ostream &out)
    {;}

protected:
  int classId;

};

class KWCharFormat : public KWCharAttribute
{
public:
  KWCharFormat() : KWCharAttribute() { classId = ID_KWCharFormat; format = 0L; }
  KWCharFormat( KWFormat* _format ) : KWCharAttribute() { classId = ID_KWCharFormat; format = _format; }
  ~KWCharFormat() { if (format) format->decRef(); format = 0L; }

  virtual KWFormat* getFormat()
    { return format; }
  virtual void setFormat(KWFormat *_format)
    { format = _format; }
  virtual bool operator==(KWCharFormat &_attrib) {
    return classId == _attrib.getClassId() &&
      (format) && *format == *_attrib.getFormat();
  }

  virtual void save(ostream &out)
    { format->save(out); }

protected:
  KWFormat *format;

};

class KWCharImage : public KWCharAttribute
{
public:
  KWCharImage() { classId = ID_KWCharImage; image = 0L; }
  KWCharImage(KWImage *_image) : KWCharAttribute() { classId = ID_KWCharImage; image = _image; }
  ~KWCharImage() { image->decRef(); image = 0L; }

  virtual KWImage* getImage()
    { return image; }
  virtual void setImage(KWImage *_image)
    { image = _image; }
  virtual void save(ostream &out)
    { image->save(out); }

protected:
  // We need the image because it has full resolution and
  // color depths. The pixmap may be reduced in both aspects
  // depending on the resolution and color depth of the display.
  KWImage *image;

};

class KWCharTab : public KWCharAttribute
{
public:
  KWCharTab() { classId = ID_KWCharTab; }

};

class KWCharVariable : public KWCharFormat
{
public:
  KWCharVariable(KWVariable *_var) : KWCharFormat() { classId = ID_KWCharVariable; var = _var; }
  ~KWCharVariable() { if (var) delete var; if (format) format->decRef(); format = 0L; }

  QString getText() { return var->getText(); }

  KWVariable *getVar() { return var; }
  
protected:
  KWVariable *var;

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
  KWString(QString _text);
  KWString(const KWString &_string);
  ~KWString()
    { free(_data_,_len_); delete [] _data_; }

  KWString &operator=(const KWString &_string);

  unsigned int size()
    { return _len_; }
  unsigned int max()
    { return _max_; }
  void append(KWChar *_text,unsigned int _len);
  void insert(unsigned int _pos,QString _text);
  void insert(unsigned int _pos,const char _c);
  void insert(unsigned int _pos,KWCharImage *_image);
  void insert(unsigned int _pos,KWCharTab *_tab);
  void insert(unsigned int _pos,KWCharVariable *_var);
  void resize(unsigned int _size,bool del = true);
  bool remove(unsigned int _pos,unsigned int _len = 1);
  KWChar* split(unsigned int _pos);

  KWChar* data()
    { return _data_; }

  QString toString(unsigned int _pos,unsigned int _len);
  void saveFormat(ostream &out);
  void loadFormat(KOMLParser&,vector<KOMLAttrib>&,KWordDocument*,KWTextFrameSet*);

  int find(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole);
  int find(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false);
  int findRev(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole);
  int findRev(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false);

protected:
  KWChar* alloc(unsigned int _size);
  void free(KWChar* _data,unsigned int _len);
  KWChar* copy(KWChar *_data,unsigned int _len);

  unsigned int _len_;
  unsigned int _max_;
  KWChar* _data_;

};

void freeChar(KWChar& _char);
ostream& operator<<(ostream &out,KWString &_string);

#endif
