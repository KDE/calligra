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
/* Module: Footnotes / Endnotes                                   */
/******************************************************************/

#include "footnote.h"

#include "kword_doc.h"
#include "fc.h"
#include "frame.h"
#include "parag.h"
#include "char.h"

/******************************************************************/
/* Class: KWFootNoteManager                                       */
/******************************************************************/

/*================================================================*/
KWFootNoteManager::KWFootNoteManager(KWordDocument *_doc)
  : start(1), superscript(true)
{
  doc = _doc;
}

/*================================================================*/
void KWFootNoteManager::recalc()
{
  KWFootNote *fn = 0L;
  int curr = start;
  for (fn = footNotes.first();fn;fn = footNotes.next())
    curr = fn->setStart(curr) + 1;
}

/*================================================================*/
int KWFootNoteManager::findStart(KWFormatContext *_fc,QPainter &p)
{
  if (_fc->getFrameSet() > 1)
    return -1;

  KWFormatContext fc(doc,_fc->getFrameSet());
  fc.init(dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(_fc->getFrameSet() - 1))->getFirstParag(),p);
  int curr = start;
  KWParag *parag = fc.getParag();

  while (parag != _fc->getParag())
    {
      KWString *str = parag->getKWString();
      for (unsigned int i = 0;i < str->size();i++)
	{
	  if (str->data()[i].attrib->getClassId() == ID_KWCharFootNote)
	    curr = dynamic_cast<KWCharFootNote*>(str->data()[i].attrib)->getFootNote()->getEnd() + 1;
	}

      parag = fc.getParag()->getNext();
    }

  if (parag)
    {
      KWString *str = parag->getKWString();
      for (unsigned int i = 0;i < str->size() && i <= _fc->getTextPos();i++)
	{
	  if (str->data()[i].attrib->getClassId() == ID_KWCharFootNote)
	    curr = dynamic_cast<KWCharFootNote*>(str->data()[i].attrib)->getFootNote()->getEnd() + 1;
	}
    }

  return curr;
}

/*================================================================*/
void KWFootNoteManager::insertFootNote(KWFootNote *fn)
{
  if (fn->getStart() == 1)
    {
      footNotes.insert(0,fn);
      recalc();
      return;
    }
  
  int i = 1;
  KWFootNote *_fn = 0L;
  for (_fn = footNotes.first();_fn;_fn = footNotes.next(),i++)
    {
      if (_fn->getEnd() != -1 && _fn->getEnd() == fn->getStart() - 1 || _fn->getStart() == fn->getStart() - 1)
	{
	  footNotes.insert(i,fn);
	  break;
	}
    }
  
  recalc();
}

/******************************************************************/
/* Class: KWFootNote                                              */
/******************************************************************/

/*================================================================*/
KWFootNote::KWFootNote(KWordDocument *_doc,QList<KWFootNoteInternal> *_parts)
  : start(1), end(1)
{
  doc = _doc;
  parts = *_parts;
  makeText();


  if (parts.isEmpty())
    warning(i18n("WARNING: Empty footnote/endnote inserted!"));
  else
    {
      start = parts.first()->from;
      end = parts.last()->to == -1 ? parts.last()->from : parts.last()->to;
    }
}

/*================================================================*/
int KWFootNote::setStart(int _start)
{
  if (parts.isEmpty()) return _start;

  int diff = _start - parts.first()->from;

  KWFootNoteInternal *fn = 0L;
  for (fn = parts.first();fn;fn = parts.next())
    {
      fn->from += diff;
      if (fn->to != -1) fn->to += diff;
    }

  makeText();

  start = parts.first()->from;
  end = parts.last()->to == -1 ? parts.last()->from : parts.last()->to;

  return end;
}

/*================================================================*/
void KWFootNote::makeText()
{
  text = before;

  KWFootNoteInternal *fn = 0L;
  for (fn = parts.first();fn;fn = parts.next())
    {
      text += QString().setNum(fn->from);
      if (fn->to != -1)
	{
	  text += fn->space;
	  text += QString().setNum(fn->to);
	}
      if (fn != parts.last())
	text += ", ";
    }

  text += after;
}

