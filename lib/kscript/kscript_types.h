#ifndef __KSCRIPT_TYPES_H__
#define __KSCRIPT_TYPES_H__

#include <qstring.h>

struct KScript
{
  typedef long Long;
  typedef double Double;
  typedef QChar Char;
  typedef bool Boolean;
  typedef QString String;
  typedef QString WString;

  struct CharRef
  {
    String* s;
    uint p;

    CharRef( String* str, uint pos) : s(str), p(pos) { }
    CharRef( const CharRef& r ) : s(r.s), p(r.p) { }

    CharRef operator=( const Char& c ) { s->ref(p)=c; return *this; }
    operator Char () const { return s->constref(p); }
  };

};

#endif
