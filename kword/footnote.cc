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

/******************************************************************/
/* Class: KWFootNoteManager                                       */
/******************************************************************/

/*================================================================*/
KWFootNoteManager::KWFootNoteManager(KWordDocument *_doc)
{
  doc = _doc;
}

/******************************************************************/
/* Class: KWFootNote                                              */
/******************************************************************/

/*================================================================*/
KWFootNote::KWFootNote(KWordDocument *_doc)
{
  doc = _doc;
}

