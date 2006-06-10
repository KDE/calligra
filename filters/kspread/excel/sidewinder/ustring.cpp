/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "ustring.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

using namespace Swinder;

UChar UChar::null;
UString::Rep UString::Rep::null = { 0, 0, 1, 0 };
UString UString::null;
static char *statBuffer = 0L;

UChar::UChar(const UCharReference &c)
    : uc( c.unicode() )
{
}

UCharReference& UCharReference::operator=(UChar c)
{
  str->detach();
  if (offset < str->rep->len)
    *(str->rep->dat + offset) = c;
  /* TODO: lengthen string ? */
  return *this;
}

UChar& UCharReference::ref() const
{
  if (offset < str->rep->len)
    return *(str->rep->dat + offset);
  else
  {
    static UChar nullRef('\0');
    return nullRef;
  }
}

namespace {
  // return an uninitialized UChar array of size s
  static inline UChar* allocateChars(int s)
  {
    // work around default UChar constructor code
    return reinterpret_cast<UChar*>(new short[s]);
  }
}


UString::Rep *UString::Rep::create(UChar *d, int l)
{
  Rep *r = new Rep;
  r->dat = d;
  r->len = l;
  r->cap = l;
  r->rc = 1;

  return r;
}

UString::Rep *UString::Rep::create(UChar *d, int l, int c)
{
  Rep *r = new Rep;
  r->dat = d;
  r->len = l;
  r->cap = c;
  r->rc = 1;

  return r;
}

UString::UString()
{
  null.rep = &Rep::null;
  attach(&Rep::null);
}

UString::UString(char c)
{
  UChar *d = allocateChars( 1 );
  d[0] = UChar(0, c);
  rep = Rep::create(d, 1);
}

UString::UString(UChar c)
{
  UChar *d = allocateChars( 1 );
  d[0] = c;
  rep = Rep::create(d, 1);
}

UString::UString(const char *c)
{
  attach(&Rep::null);
  operator=(c);
}

UString::UString(const UChar *c, int length)
{
  UChar *d = allocateChars( length );
  memcpy(d, c, length * sizeof(UChar));
  rep = Rep::create(d, length);
}

UString::UString(UChar *c, int length, bool copy)
{
  UChar *d;
  if (copy) {
    d = allocateChars( length );
    memcpy(d, c, length * sizeof(UChar));
  } else
    d = c;
  rep = Rep::create(d, length);
}

UString::UString(const UString &b)
{
  attach(b.rep);
}

// NOTE: this is private, be careful when using it !
UString::UString(Rep *r): rep(r)
{
}


UString::~UString()
{
  release();
}

UString UString::number(int i)
{
#if 0
  // standard and safe way
  char buf[1+sizeof(int)*3];
  snprintf(buf, sizeof(int)*3, "%d", i);
  buf[sizeof(int)*3] = '\0';
  return UString(buf);
#else
  // micro-optimized version
  static unsigned short digits[] = 
    { '9', '8', '7', '6', '5', '4', '3', '2', '1', 
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
      
  UString::Rep* rep = 0;
      
  if(i == 0)
  {
    UChar* buf = allocateChars(1);
    buf[0] = '0';
    rep = Rep::create(buf, 1);
  }
  else
  {
    bool neg = (i < 0);
    int ndig = 1 + sizeof(int)*3;

    UChar* buf = allocateChars(ndig);
    UChar* ptr = buf + ndig - 1;
    int len = (neg) ? 1 : 0;

    // construct all the digits
    for(; i != 0; i/=10, len++) 
      *ptr-- = digits[9+i%10];

    if(neg) 
      *ptr-- = '-';  
  
    // shift, don't use memcpy because overlapping area
    memmove(buf, ptr+1, len*sizeof(unsigned short));
    
    rep = Rep::create(buf, len, ndig);
  }
  
  return UString(rep);
#endif
}

UString UString::number(unsigned int u)
{
#if 0
  // standard and safe way
  char buf[1+sizeof(unsigned int)*3];
  snprintf(buf, sizeof(unsigned int)*3, "%d", u);
  buf[sizeof(int)*3] = '\0';
  return UString(buf);
#else
  // micro-optimized version
  static unsigned short digits[] = 
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

  UString::Rep* rep = 0;
      
  if(u < 10)
  {
    UChar* buf = allocateChars(1);
    buf[0] = digits[u];
    rep = Rep::create(buf, 1);
  }
  else
  {
    int ndig = 1 + sizeof(int)*3;
    UChar* buf = allocateChars(ndig);
    UChar* ptr = buf + ndig - 1;
    int len = 0;

    // construct all the digits
    for(; u != 0; u/=10, len++) 
      *ptr-- = digits[u%10];
  
    // shift, don't use memcpy because overlapping area
    memmove(buf, ptr+1, len*sizeof(unsigned short));
    
    rep = Rep::create(buf, len, ndig);
  }
  
  return UString(rep);
#endif
}

UString UString::number(double d)
{
  char buf[40];
  snprintf(buf, 39, "%.16g", d);
  buf[sizeof(int)*3] = '\0';
  return UString(buf);
}

void UString::truncate(int n)
{
  if((n >= 0) && (n < length()))
  {
    detach();
    rep->len = n;
  }
}

void UString::reserve(int r)
{
  if(r > length())
  {
    int l = length();
    UChar* n = allocateChars( r );
    memcpy(n, data(), l * sizeof(UChar));
    release();
    rep = Rep::create(n, l, r); 
  }
}

UString &UString::append(const UString &t)
{
  int tl = t.length();
  if(tl > 0)
  {
    detach();
    int l = length();
    
    // no space, we have to reallocate first
    if(capacity() < tl + l)
      reserve(tl +l);
      
    UChar *dd = rep->data();
    memcpy(dd+l, t.data(), tl * sizeof(UChar));
    rep->len += tl;
  }

  return *this;
}

UString &UString::append(const char* s)
{
  int tl = strlen(s);
  if(tl > 0)
  {
    detach();
    int l = length();
    
    // no space, we have to reallocate first
    if(capacity() < tl + l)
      reserve(tl +l);

    // copy each character        
    UChar *dd = rep->data();
    for (int i = 0; i < tl; i++)
      dd[l+i].uc = static_cast<unsigned char>( s[i] );
    rep->len += tl;
  }

  return *this;
}

UString &UString::append(UChar c)
{
  detach();
  int l = length();
  
  // we need to reallocate
  // in case another append follows, so let's reserve()
  // this avoids subsequent expensive reallocation
  if(capacity() < l + 1)
    reserve(l + 8);
  
  UChar *dd = rep->data();
  dd[l] = c;
  rep->len++;

  return *this;
}

UString &UString::append(char c)
{
  return append( UChar((unsigned short)c) );
}


UString &UString::prepend(const UString &t)
{
  int tl = t.length();
  if(tl > 0)
  {
    int l = length();
    
    // no space, we have to reallocate first
    if(capacity() < tl + l)
      reserve(tl +l);

    // shift the string, then place the new string      
    UChar *dd = rep->data();
    for(int i = l-1; i >= 0; i--)
      dd[i+tl] = dd[i];
    memcpy(dd, t.data(), tl * sizeof(UChar));
    rep->len += tl;
  }

  return *this;
}

UString &UString::prepend(const char* s)
{
  int tl = strlen(s);
  if(tl > 0)
  {
    int l = length();
    
    // no space, we have to reallocate first
    if(capacity() < tl + l)
      reserve(tl +l);

    // shift the string, then copy each new character        
    UChar *dd = rep->data();
    for(int i = l-1; i >= 0; i--)
      dd[i+tl] = dd[i];
    for(int j = 0; j < tl; j++)
      dd[j].uc = static_cast<unsigned char>( s[j] );
    rep->len += tl;
  }

  return *this;
}

UString &UString::prepend(UChar c)
{
  int l = length();
  
  // we need to reallocate and reserve
  // see also append(UChar c) function
  if(capacity() < l + 1)
    reserve(l + 8);
  
  UChar *dd = rep->data();
  for(int i = l-1; i >= 0; i--)
    dd[i+1] = dd[i];
  dd[0] = c;
  rep->len++;

  return *this;
}

UString &UString::prepend(char c)
{
  return prepend( UChar((unsigned short)c) );
}

char *UString::ascii() const
{
  if (statBuffer)
    delete [] statBuffer;

  statBuffer = new char[length()+1];
  for(int i = 0; i < length(); i++)
    statBuffer[i] = data()[i].low();
  statBuffer[length()] = '\0';

  return statBuffer;
}

UString &UString::operator=(const char *c)
{
  release();
  int l = c ? strlen(c) : 0;
  UChar *d = allocateChars( l );
  for (int i = 0; i < l; i++)
    d[i].uc = static_cast<unsigned char>( c[i] );
  rep = Rep::create(d, l);

  return *this;
}

UString &UString::operator=(const UString &str)
{
  str.rep->ref();
  release();
  rep=str.rep;

  return *this;
}

UString &UString::operator+=(const UString &s)
{
  return append(s);
}

bool UString::is8Bit() const
{
  const UChar *u = data();
  for(int i = 0; i < length(); i++, u++)
    if (u->uc > 0xFF)
      return false;

  return true;
}

UChar UString::operator[](int pos) const
{
  if (pos >= length())
    return UChar::null;

  return static_cast<const UChar *>( data() )[pos];
}

UCharReference UString::operator[](int pos)
{
  /* TODO: boundary check */
  return UCharReference(this, pos);
}

UString UString::substr(int pos, int len) const
{
  if (isNull())
    return UString();
  if (pos < 0)
    pos = 0;
  else if (pos >= static_cast<int>( length() ))
    pos = length();
  if (len < 0)
    len = length();
  if (pos + len >= static_cast<int>( length() ))
    len = length() - pos;

  UChar *tmp = allocateChars( len );
  memcpy(tmp, data()+pos, len * sizeof(UChar));
  UString result(tmp, len);
  delete [] tmp;

  return result;
}

int UString::find(const UString &f, int pos) const
{
  if (isNull())
    return -1;
  long fsize = f.length() * sizeof(UChar);
  if (pos < 0)
    pos = 0;
  const UChar *end = data() + length() - f.length();
  for (const UChar *c = data() + pos; c <= end; c++)
    if (!memcmp(c, f.data(), fsize))
      return (c-data());

  return -1;
}

void UString::attach(Rep *r)
{
  rep = r;
  rep->ref();
}

void UString::detach()
{
  if (rep->rc > 1) 
  {
    int c = capacity();
    int l = length();
    UChar *n = allocateChars( c );
    memcpy(n, data(), l * sizeof(UChar));
    release();
    rep = Rep::create(n, l, c);
  }
}

void UString::release()
{
  if (!rep->deref()) 
  {
    delete [] rep->dat;
    delete rep;
  }
}

bool Swinder::operator==(const UString& s1, const UString& s2)
{
  if (s1.rep->len != s2.rep->len)
    return false;

  return (memcmp(s1.rep->dat, s2.rep->dat,
		 s1.rep->len * sizeof(UChar)) == 0);
}

bool Swinder::operator==(const UString& s1, const char *s2)
{
  if (s2 == 0L)
    return s1.isEmpty();

  if (s1.length() != static_cast<int>( strlen(s2) ))
    return false;

  const UChar *u = s1.data();
  while (*s2) 
  {
    if (u->uc != *s2 )
      return false;
    s2++;
    u++;
  }

  return true;
}

bool Swinder::operator<(const UString& s1, const UString& s2)
{
  const int l1 = s1.length();
  const int l2 = s2.length();
  const int lmin = l1 < l2 ? l1 : l2;
  const UChar *c1 = s1.data();
  const UChar *c2 = s2.data();
  int l = 0;
  while (l < lmin && *c1 == *c2) 
  {
    c1++;
    c2++;
    l++;
  }
  if (l < lmin)
    return (c1->unicode() < c2->unicode());

  return (l1 < l2);
}

UString Swinder::operator+(const UString& s1, const UString& s2)
{
  UString tmp(s1);
  tmp.append(s2);

  return tmp;
}


UConstString::UConstString( UChar* data, unsigned int length ) : 
UString( data, length, false )
{
}

UConstString::~UConstString()
{
  if ( rep->rc > 1 ) {
    int l = length();
    UChar* n = allocateChars( l );
    memcpy( n, data(), l * sizeof( UChar ) );
    rep->dat = n;
  }
  else
    rep->dat = 0;
}
