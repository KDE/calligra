/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: KWAutoFormat (header)                                  */
/******************************************************************/

#ifndef kwautoformat_h
#define kwautoformat_h

#include <qstring.h>
#include <qcolor.h>

#include "format.h"

class KWordDocument;
class KWFormatContext;
class KWParag;

/******************************************************************/
/* Class: KWAutoFormatEntry                                       */
/******************************************************************/

class KWAutoFormatEntry
{
public:
  KWAutoFormatEntry();

protected:
  QString expr;
  bool checkFamily,checkColor,checkSize,checkBold,checkItalic,checkUnderline,checkVertAlign;
  QString family;
  QColor color;
  int size;
  bool bold,italic,underline;
  KWFormat::VertAlign vertAlign;
  bool caseSensitive,regexp,wildcard;

};

/******************************************************************/
/* Class: KWAutoFormat                                            */
/******************************************************************/

class KWAutoFormat
{
public:
  struct TypographicQuotes
  {
    TypographicQuotes() : begin('»'), end('«'), replace(true)
    {}
    
    QChar begin,end;
    bool replace;
  };
  
  KWAutoFormat(KWordDocument *_doc);

  bool doAutoFormat(KWParag *parag,KWFormatContext *fc);
  bool doTypographicQuotes(KWParag *parag,KWFormatContext *fc);

protected:
  KWordDocument *doc;
  TypographicQuotes typographicQuotes;
  
};

#endif
