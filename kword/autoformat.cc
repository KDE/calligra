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

#include "autoformat.h"
#include "parag.h"
#include "kword_doc.h"
#include "fc.h"

/******************************************************************/
/* Class: KWAutoFormatEntry                                       */
/******************************************************************/

/*================================================================*/
KWAutoFormatEntry::KWAutoFormatEntry() 
{
  checkFamily = checkColor = checkSize = checkBold = checkItalic = checkUnderline = checkVertAlign = false;
  expr = "";
  family = "times";
  color = Qt::black;
  size = 12;
  bold = italic = underline = false;
  vertAlign = KWFormat::VA_NORMAL;
  caseSensitive = false;
  regexp = false;
  wildcard = false;
}

/******************************************************************/
/* Class: KWAutoFormat                                            */
/******************************************************************/

/*================================================================*/
KWAutoFormat::KWAutoFormat(KWordDocument *_doc) 
{ 
  doc = _doc; 
}

/*================================================================*/
bool KWAutoFormat::doAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  return false;
}

/*================================================================*/
bool KWAutoFormat::doTypographicQuotes(KWParag *parag,KWFormatContext *fc)
{
  return false;
}
