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
#include "framedia.h"

#include <qevent.h>
#include <math.h>
#include <qwidget.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qcursor.h>

#include <X11/Xlib.h>
#include <kapp.h>

class KWordGUI;
class KWordDocument_impl;

enum MouseMode {MM_EDIT = 0,MM_EDIT_FRAME = 1,MM_CREATE_TEXT = 2,MM_CREATE_PIX = 3};

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
    { fc->getParag()->getParagLayout()->setMMParagHeadOffset(static_cast<unsigned int>(_before));
    recalcAll = true; recalcCursor(); recalcAll = false; }
  void setSpaceAfterParag(float _after)
    { fc->getParag()->getParagLayout()->setMMParagFootOffset(static_cast<unsigned int>(_after));
    recalcAll = true; recalcCursor(); recalcAll = false; }
  void setLineSpacing(unsigned int _spacing)
    { fc->getParag()->getParagLayout()->setPTLineSpacing(_spacing);
    recalcAll = true; recalcCursor(); recalcAll = false; }

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
  void setMouseMode(MouseMode _mm);
  int getPageOfRect(QRect _rect);
  
public slots:
  void newLeftIndent(int _left); 
  void newFirstIndent(int _first);
  void frameSizeChanged(KoPageLayout);
  void mmEdit()
    { setMouseMode(MM_EDIT); mmUncheckAll(); mm_menu->setItemChecked(mm_edit,true); }
  void mmEditFrame()
    { setMouseMode(MM_EDIT_FRAME); mmUncheckAll(); mm_menu->setItemChecked(mm_edit_frame,true); }
  void mmCreateText()
    { setMouseMode(MM_CREATE_TEXT); mmUncheckAll(); mm_menu->setItemChecked(mm_create_text,true); }
  void mmCreatePix()
    { setMouseMode(MM_CREATE_PIX); mmUncheckAll(); mm_menu->setItemChecked(mm_create_pix,true); }
  void femProps();

protected slots:
  void frameDiaClosed()
    { hiliteFrameSet = -1; recalcAll = true; recalcText(); recalcCursor(); recalcAll = false; }

protected:
  unsigned int ptLeftBorder();
  unsigned int ptRightBorder();
  unsigned int ptTopBorder();
  unsigned int ptBottomBorder();
  unsigned int ptPaperWidth();
  unsigned int ptPaperHeight();
  unsigned int ptColumnWidth();
  unsigned int ptColumnSpacing();
  unsigned int mmPaperWidth();
  unsigned int mmPaperHeight();

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
  void setupMenus();
  void mmUncheckAll();

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
  bool recalcAll;

  MouseMode mouseMode;
  QPopupMenu *mm_menu,*frame_edit_menu;
  int mm_edit,mm_edit_frame,mm_create_text,mm_create_pix;

  int oldMx,oldMy;
  bool deleteMovingRect;

  int hiliteFrameSet;

  KWFrameDia *frameDia;

};

#endif





