/* This file is part of the KDE libraries
    Copyright (C) 1999 Ilya Baran (ibaran@mit.edu)

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

#ifndef KFORMULAEDIT_H_INCLUDED
#define KFORMULAEDIT_H_INCLUDED

#include "kformula.h"
#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qstack.h>
#include <qsize.h>

class box;
class KFormula;
struct charinfo;

// now the fake chars used for control for passing to insertChar:
#define CUT_CHAR 0
#define COPY_CHAR 1
#define PASTE_CHAR 2
#define UNDO_CHAR 3
#define REDO_CHAR 4


struct _cursorInfo { //private
  int dirty;
  QRect pos;
};

/**
 * This guy is a formula editor--kind of like a line editor but
 * you can do formulas.  If you construct it with restricted
 * set to true, it prevents you from typing things it doesn't know
 * how to evaluate (like matrices, greek, integrals, etc.)
 * If you want to use your own evaluation, but want to have
 * the typing restrictions, you can use uglyForm() on a restricted formula
 * to get a c-style expression and setExtraChars() to allow characters
 * beyond the default allowed in restricted mode.
 *
 * Features of KFormulaEdit include infinite undo/redo, cut/copy/paste,
 * and you can enter almost anything KFormula can display (I think).
 * To save a formula, use text(), save the returned string, and when
 * loading use setText(QString) with the saved string.
 *
 * Keyboard shortcuts are availiable, but it's probably easier to
 * use a KFormulaToolBar (which see).
 * @short A WYSIWYG formula editor widget.
 * @author Ilya Baran <ibaran@mit.edu>
 * @version $Id$
 */
class KFormulaEdit : public QWidget
{
  Q_OBJECT
public:
  KFormulaEdit(QWidget * parent=0, const char *name=0, WFlags f=0, bool restricted = false);
  virtual ~KFormulaEdit();
  void setText(QString text);
  void setExtraChars(QString c) { extraChars = c; }
  QString text() const { return formText; }
  void redraw(int all = 1);
  KFormula *getFormula() const { return form; }
  QString uglyForm() const;
  void setUglyForm(QString ugly); // try to convert an ugly form to be less so. 

  virtual QSize sizeHint() const;
  virtual QSizePolicy sizePolicy() const;

  /**
   * By default this is turned off for performance reasons.
   * If turned on, the signal @ref #sizeHint is emitted everytime
   * the formula gets a new size.
   */
  void enableSizeHintSignal( bool b) { sendSizeHint = b; }

signals:
  void sizeHint( QSize );
  void formulaChanged( const QString & );

private:
  class KFormulaEditPrivate;
  KFormulaEditPrivate *d;
  QArray<_cursorInfo> cursorCache;
  bool restricted;  //whether for typesetting or evaluation
  QRect oldBound; // previous rectangle of formula
  QString extraChars;
  int cacheState;
  QPixmap pm;       //double buffering
  KFormula *form;
  QString formText; //the text that gets converted into the formula
  static QString clipText; //clipboard
  int cursorPos;
  QArray<charinfo> info; //where each character of formText
                         //ends up in the formula
  QStack<QString> undo_stack;
  QStack<QString> redo_stack;  //if there was a "diff" for strings it would
                               //save space
  QTimer t;
  QTimer fast; //t is for the cursor, fast for the cache

  void keyPressEvent(QKeyEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  void focusInEvent(QFocusEvent *e);
  void focusOutEvent(QFocusEvent *e);
  void expandSelection();

  int isValidCursorPos(int pos);
  int isInMatrix(int immed = 0); // am I inside (directly inside if immed)
                                 // of a matrix?
  int deleteAtCursor();
  int posAtPoint(QPoint p);
  int cursorDrawn;
  int textSelected;
  int selectStart;
  int selectStartOrig;
  int isInString(int pos, const QString &str) const;

  QRect getCursorPos(int pos);

  bool nextGreek;

  bool sendSizeHint;

  void do_undo(QString oldText, int oldc);
  void do_redo(QString oldText, int oldc);
  void do_cut(QString oldText, int oldc);
  void do_copy(QString oldText, int oldc);
  void do_paste(QString oldText, int oldc);

private:
  void insertChar(QChar c);

public slots:
  void insertChar(int c); // just to make connecting to toolbars easy.

private slots:
  void toggleCursor();
  void computeCache();   // computes one step

};


#endif // KFORMULAEDIT_H_INCLUDED




















