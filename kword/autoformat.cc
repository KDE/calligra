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
#include "char.h"

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
  : typographicQuotes(), enabled(false), lastWasDotSpace(false),
    convertUpperCase(true), lastWasUpper(false), convertUpperUpper(false)
{
  doc = _doc;
  tmpBuffer = 0L;
}

/*================================================================*/
void KWAutoFormat::startAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return;

  lastWasDotSpace = false;
  lastWasUpper = false;

  tmpBuffer = new KWString(doc);
}

/*================================================================*/
bool KWAutoFormat::doAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return false;

  tmpBuffer->append(parag->getKWString()->data()[fc->getTextPos()]);

  return false;
}

/*================================================================*/
void KWAutoFormat::endAutoFormat(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return;

  if (tmpBuffer)
    delete tmpBuffer;
  tmpBuffer = 0L;
}

/*================================================================*/
bool KWAutoFormat::doTypographicQuotes(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return false;
  
  if (!typographicQuotes.replace)
    {
      if (parag->getKWString()->data()[fc->getTextPos()].autoformat && 
	  parag->getKWString()->data()[fc->getTextPos()].autoformat->type == AT_TypographicQuotes)
	{
	  parag->getKWString()->data()[fc->getTextPos()].c = parag->getKWString()->data()[fc->getTextPos()].autoformat->c;
	  delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
	  parag->getKWString()->data()[fc->getTextPos()].autoformat = 0L;
	}
      return true;
    }
  
  if (parag->getKWString()->data()[fc->getTextPos()].c == QChar('\"') ||
      parag->getKWString()->data()[fc->getTextPos()].c == typographicQuotes.begin ||
      parag->getKWString()->data()[fc->getTextPos()].c == typographicQuotes.end)
    {
      if (fc->getTextPos() == 0 || fc->getTextPos() > 0 &&
	  parag->getKWString()->data()[fc->getTextPos() - 1].c == QChar(' '))
	{
	  if (parag->getKWString()->data()[fc->getTextPos()].autoformat)
	    delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
	  
	  AutoformatInfo *info = new AutoformatInfo;
	  info->c = QChar('\"');
	  info->type = AT_TypographicQuotes;
	  
	  parag->getKWString()->data()[fc->getTextPos()].autoformat = info;

	  parag->getKWString()->data()[fc->getTextPos()].c = typographicQuotes.begin;
	}
      else
	{
	  if (parag->getKWString()->data()[fc->getTextPos()].autoformat)
	    delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
	  
	  AutoformatInfo *info = new AutoformatInfo;
	  info->c = QChar('\"');
	  info->type = AT_TypographicQuotes;
	  
	  parag->getKWString()->data()[fc->getTextPos()].autoformat = info;

	  parag->getKWString()->data()[fc->getTextPos()].c = typographicQuotes.end;
	}
      
      return true;
    }

  return false;
}

/*================================================================*/
bool KWAutoFormat::doUpperCase(KWParag *parag,KWFormatContext *fc)
{
  if (!enabled)
    return false;

  bool converted = false;

  if (convertUpperCase)
    {
      if (lastWasDotSpace &&
	  parag->getKWString()->data()[fc->getTextPos()].c != QChar('.') &&
	  parag->getKWString()->data()[fc->getTextPos()].c != QChar(' ') &&
	  isLower(parag->getKWString()->data()[fc->getTextPos()].c))
	{
	  if (parag->getKWString()->data()[fc->getTextPos()].autoformat)
	    delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
	  
	  AutoformatInfo *info = new AutoformatInfo;
	  info->c = parag->getKWString()->data()[fc->getTextPos()].c;
	  info->type = AT_UpperCase;
	  
	  parag->getKWString()->data()[fc->getTextPos()].autoformat = info;

	  parag->getKWString()->data()[fc->getTextPos()].c.toUpper();
	  converted = true;
	}
    }
  else if (parag->getKWString()->data()[fc->getTextPos()].autoformat &&
	   parag->getKWString()->data()[fc->getTextPos()].autoformat->type == AT_UpperCase)
    {
      parag->getKWString()->data()[fc->getTextPos()].c = parag->getKWString()->data()[fc->getTextPos()].autoformat->c;
      delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
      parag->getKWString()->data()[fc->getTextPos()].autoformat = 0L;
    }

  if (convertUpperUpper)
    {
      if (!lastWasDotSpace && lastWasUpper &&
	  isUpper(parag->getKWString()->data()[fc->getTextPos()].c))
	{
	  if (parag->getKWString()->data()[fc->getTextPos()].autoformat)
	    delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
	  
	  AutoformatInfo *info = new AutoformatInfo;
	  info->c = parag->getKWString()->data()[fc->getTextPos()].c;
	  info->type = AT_UpperUpper;
	  
	  parag->getKWString()->data()[fc->getTextPos()].autoformat = info;

	  parag->getKWString()->data()[fc->getTextPos()].c.toLower();
	  converted = true;
	}
    }
  else if (parag->getKWString()->data()[fc->getTextPos()].autoformat &&
	   parag->getKWString()->data()[fc->getTextPos()].autoformat->type == AT_UpperUpper)
    {
      parag->getKWString()->data()[fc->getTextPos()].c = parag->getKWString()->data()[fc->getTextPos()].autoformat->c;
      delete parag->getKWString()->data()[fc->getTextPos()].autoformat;
      parag->getKWString()->data()[fc->getTextPos()].autoformat = 0L;
    }

  if (convertUpperUpper || convertUpperCase)
    {
      if (parag->getKWString()->data()[fc->getTextPos()].c == QChar('.'))
	lastWasDotSpace = true;
      else if (parag->getKWString()->data()[fc->getTextPos()].c != QChar('.') &&
	       parag->getKWString()->data()[fc->getTextPos()].c != QChar(' '))
	lastWasDotSpace = false;
    }

  if (convertUpperUpper)
    {
      if (isUpper(parag->getKWString()->data()[fc->getTextPos()].c))
	lastWasUpper = true;
      else
	lastWasUpper = false;
    }

  return converted;
}

/*================================================================*/
void KWAutoFormat::configTypographicQuotes(TypographicQuotes _tq)
{
  typographicQuotes = _tq;
}

/*================================================================*/
void KWAutoFormat::configUpperCase(bool _uc)
{
  convertUpperCase = _uc;
}

/*================================================================*/
void KWAutoFormat::configUpperUpper(bool _uu)
{
  convertUpperUpper = _uu;
}

/*================================================================*/
bool KWAutoFormat::isUpper(const QChar &c)
{
  QChar c2(c);
  c2.toLower();

  if (c2 != c)
    return true;
  else
    return false;
}

/*================================================================*/
bool KWAutoFormat::isLower(const QChar &c)
{
  QChar c2(c);
  c2.toUpper();

  if (c2 != c)
    return true;
  else
    return false;
}
