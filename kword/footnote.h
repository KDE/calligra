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

#ifndef footnote_h
#define footnote_h

#include <qlist.h>
#include <qpainter.h>

class KWFootNote;
class KWordDocument;
class KWFormatContext;

/******************************************************************/
/* Class: KWFootNoteManager                                       */
/******************************************************************/

class KWFootNoteManager
{
public:
  KWFootNoteManager(KWordDocument *_doc);

  int getStart() { return start; }
  void setStart(int s) { start = s; recalc(); }

  void recalc();

  int findStart(KWFormatContext *_fc,QPainter &p);

  bool showFootNotesSuperscript() { return superscript; }
  void setShowFootNotesSuperscript(bool _s) { superscript = _s; }

  void insertFootNote(KWFootNote *fn);
  
protected:
  KWordDocument *doc;
  int start;
  QList<KWFootNote> footNotes;
  bool superscript;

};

/******************************************************************/
/* Class: KWFootNote                                              */
/******************************************************************/

class KWFootNote
{
public:
  struct KWFootNoteInternal
  {
    int from;
    int to;
    QString space;
  };

  KWFootNote(KWordDocument *_doc,QList<KWFootNoteInternal> *_parts);

  KWFootNote *copy() { return new KWFootNote(doc,new QList<KWFootNoteInternal>(parts)); }

  int getStart() { return start; }
  int getEnd() { return end; }

  QString getText() { return text; }

  /**
   * returns new end
   */
  int setStart(int _start);

  void setBefore(const QString &_before) { before = _before; }
  void setAfter(const QString &_after) { after = _after; }
  
protected:
  void makeText();

  KWordDocument *doc;
  int start,end;
  QList<KWFootNoteInternal> parts;
  QString before,after,text;

};

#endif
