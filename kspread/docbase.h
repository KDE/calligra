/* This file is part of the KDE project
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef DOCBASE
#define DOCBASE

class KLocale;
class KSpreadDoc;
class KSpreadMap;
class KSpreadStyleManager;

namespace KSpread {

class ValueCalc;
class ValueConverter;
class ValueFormatter;
class ValueParser;

// used by DocBase and KSpreadDoc - do not use elsewhere
struct DocInfo {
  KSpreadDoc *doc;
  KLocale *locale;
  KSpreadMap *map;
  KSpreadStyleManager *styleManager;
  ValueParser *parser;
  ValueFormatter *formatter;
  ValueConverter *converter;
  ValueCalc *calc;
};

/**
The DocBase class is a base for all classes that are a part of a document,
and need to have access to other classes within the document.
To avoid having to spread pointers across many classes, one class exists,
where all the pointers are available in one place.

In addition, the class also provides access to some common functions,
so that other classes don't need to put in lots of #include-s.
*/

class DocBase {
 public:
  DocBase (DocInfo *priv);
  
  KSpreadDoc *doc () const { return di->doc; };
  KLocale *locale () const { return di->locale; };
  KSpreadMap *map () const { return di->map; };
  KSpreadStyleManager *styleManager () const { return di->styleManager; };
  KSpread::ValueParser *parser () const { return di->parser; };
  KSpread::ValueFormatter *formatter () const { return di->formatter; };
  KSpread::ValueConverter *converter () const { return di->converter; };
  KSpread::ValueCalc *calc () const { return di->calc; };
  
  void emitBeginOperation (bool waitCursor = true);
  void emitEndOperation ();

 protected:
  DocInfo *di;
};

};

#endif  //DOCBASE

