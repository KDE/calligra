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
/* Module: Page (header)                                          */
/******************************************************************/

#ifndef kword_page_h
#define kword_page_h

#include "defs.h"
#include "fc.h"
#include "format.h"
#include "paraglayout.h"
#include "char.h"
#include "frame.h"

#include <qevent.h>
#include <math.h>
#include <qwidget.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qclipboard.h>

#include <X11/Xlib.h>
#include <kapp.h>

class KWordGUI;
class KWordDocument_impl;

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

class KWPage : public QWidget
{
  Q_OBJECT
  
public:
  KWPage(QWidget *parent,KWordDocument_impl *_doc,KWordGUI *_gui);

  void setDocument(KWordDocument_impl *_doc)
    { doc = _doc; }

  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent* e);
  void keyPressEvent(QKeyEvent * e);
  void resizeEvent(QResizeEvent *e);

  /**
   * @return 0 if the cursor is visible with respect to the 
   *         y-coordinate. Mention that the cursor may be never the less
   *         invisible because of its x-coordinate. 1 is returned if the cursor
   *         is under the visible area and -1 if the cursor is above.
   *
   * @see #isCursorXVisible
   */
  int isCursorYVisible(KWFormatContext &_fc);
  /**
   * @see #isCursorYVisible
   */
  int isCursorXVisible(KWFormatContext &_fc);
  
  void scrollToCursor(KWFormatContext &_fc);
  void scrollToOffset(int _x,int _y,KWFormatContext &_fc);
 
  void setXOffset(int _x)
    { xOffset = _x; calcVisiblePages(); }
  void setYOffset(int _y)
    { yOffset = _y; calcVisiblePages(); }

  void scroll(int dx,int dy);

  void formatChanged(KWFormat &_format);
  void setFlow(KWParagLayout::Flow _flow)
    { fc->getParag()->getParagLayout()->setFlow(_flow); recalcCursor(); }
  void setLeftIndent(float _left)
    { fc->getParag()->getParagLayout()->setMMLeftIndent(static_cast<unsigned int>(_left)); recalcCursor(); }
  void setFirstLineIndent(float _first)
    { fc->getParag()->getParagLayout()->setMMFirstLineLeftIndent(static_cast<unsigned int>(_first)); recalcCursor(); }
  void setSpaceBeforeParag(float _before)
    { fc->getParag()->getParagLayout()->setMMParagHeadOffset(static_cast<unsigned int>(_before)); recalcCursor(); }
  void setSpaceAfterParag(float _after)
    { fc->getParag()->getParagLayout()->setMMParagFootOffset(static_cast<unsigned int>(_after)); recalcCursor(); }
  void setLineSpacing(unsigned int _spacing)
    { fc->getParag()->getParagLayout()->setPTLineSpacing(_spacing); recalcCursor(); }

  void setParagLeftBorder(KWParagLayout::Border _brd)
    { fc->getParag()->getParagLayout()->setLeftBorder(_brd); recalcCursor(); }
  void setParagRightBorder(KWParagLayout::Border _brd)
    { fc->getParag()->getParagLayout()->setRightBorder(_brd); recalcCursor(); }
  void setParagTopBorder(KWParagLayout::Border _brd)
    { fc->getParag()->getParagLayout()->setTopBorder(_brd); recalcCursor(); }
  void setParagBottomBorder(KWParagLayout::Border _brd)
    { fc->getParag()->getParagLayout()->setBottomBorder(_brd); recalcCursor(); }

  KWParagLayout::Flow getFlow()
    { return fc->getParag()->getParagLayout()->getFlow(); }
  float getLeftIndent()
    { return static_cast<float>(fc->getParag()->getParagLayout()->getMMLeftIndent()); }
  float getFirstLineIndent()
    { return static_cast<float>(fc->getParag()->getParagLayout()->getMMFirstLineLeftIndent()); }
  float getSpaceBeforeParag()
    { return static_cast<float>(fc->getParag()->getParagLayout()->getMMParagHeadOffset()); }
  float getSpaceAfterParag()
    { return static_cast<float>(fc->getParag()->getParagLayout()->getMMParagFootOffset()); }
  unsigned int getLineSpacing()
    { return fc->getParag()->getParagLayout()->getPTLineSpacing(); }


  void recalcCursor(bool _repaint = true,int _pos = -1,KWFormatContext *_fc = 0L);

  int getVertRulerPos();

  void insertPictureAsChar(QString _filename);

  void editCut();
  void editCopy();
  void editPaste(QString _string);

  void recalcText();
  void drawBorders(QPainter &_painter,QRect v_area);
  void setRuler2Frame(unsigned int _frameset,unsigned int _frame);
  
public slots:
  void newLeftIndent(int _left)
    { setLeftIndent(static_cast<float>(_left)); }
  void newFirstIndent(int _first)
    { setFirstLineIndent(static_cast<float>(_first)); }
  void frameSizeChanged(KoPageLayout);

protected:
  unsigned int ptLeftBorder();
  unsigned int ptRightBorder();
  unsigned int ptTopBorder();
  unsigned int ptBottomBorder();
  unsigned int ptPaperWidth();
  unsigned int ptPaperHeight();
  unsigned int ptColumnWidth();
  unsigned int ptColumnSpacing();

  void enterEvent(QEvent *)
    { setFocus(); }
  void focusInEvent(QFocusEvent *) {}
  void focusOutEvent(QFocusEvent *) {}

  /**
   * Looks at 'yOffset' and 'paperHeight' and calculates the first and
   * last visible pages. The values are stored in 'firstVisiblePage' and
   * 'lastVisiblePages'.
   *
   * @see #yOffset
   * @see #paperHeight
   * @see #firstVisiblePage
   * @see #lastVisiblePage
   */
  void calcVisiblePages();

  void drawBuffer();
  void drawBuffer(QRect _rect);
  void copyBuffer();

  KWordDocument_impl *doc;
  bool markerIsVisible;
  bool paint_directly,has_to_copy;

  /**
   * The xOffset in zoomed pixels.
   */
  unsigned int xOffset;
  /**
   * The yOffset in zoomed pixels.
   */
  unsigned int yOffset;

  /**
   * The first (partial?) visible page.
   *
   * @see #calcVisiblePages
   */
  unsigned int firstVisiblePage;
  /**
   * The last (partial?) visible page.
   *
   * @see #calcVisiblePages
   */
  unsigned int lastVisiblePage;
  
  KWFormatContext *fc;

  KWordGUI *gui;
  QPixmap buffer;
  KWFormat format;
  
  bool mousePressed;
  bool inKeyEvent;
  
};

#endif





