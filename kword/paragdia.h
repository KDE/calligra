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
/* Module: Paragraph Dialog (header)                              */
/******************************************************************/

#ifndef paragdia_h
#define paragdia_h

#include <qtabdialog.h>
#include <qstrlist.h>
#include <qlist.h>

#include "paraglayout.h"

class KWordDocument; 
class QWidget; 
class QGridLayout; 
class KRestrictedLine; 
class QLabel; 
class QGroupBox; 
class QComboBox; 
class QRadioButton; 
class KButtonBox; 
class QPushButton; 
class KColorDialog; 
class KColorButton; 
class QSpinBox; 
class QButtonGroup; 
class QListBox; 

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

class KWPagePreview : public QGroupBox
{
	Q_OBJECT

public:
	KWPagePreview( QWidget*, const char* ); 
	~KWPagePreview() {}

	void setLeft( float _left )
    { left = _left; repaint( false ); }
	void setRight( float _right )
    { right = _right; repaint( false ); }
	void setFirst( float _first )
    { first = _first; repaint( false ); }
	void setSpacing( float _spacing )
    { spacing = _spacing; repaint( false ); }
	void setBefore( float _before )
    { before = _before; repaint( false ); }
	void setAfter( float _after )
    { after = _after; repaint( false ); }

protected:
	void drawContents( QPainter* ); 

	float left, right, first, spacing, before, after; 

}; 

/******************************************************************/
/* class KWPagePreview2                                           */
/******************************************************************/

class KWPagePreview2 : public QGroupBox
{
	Q_OBJECT

public:
	KWPagePreview2( QWidget*, const char* ); 
	~KWPagePreview2() {}

	void setFlow( KWParagLayout::Flow _flow )
    { flow = _flow; repaint( false ); }

protected:
	void drawContents( QPainter* ); 

	KWParagLayout::Flow flow; 

}; 

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/

class KWBorderPreview : public QGroupBox
{
	Q_OBJECT

public:
	KWBorderPreview( QWidget*, const char* ); 
	~KWBorderPreview() {}

	KWParagLayout::Border getLeftBorder() { return leftBorder; }
	void setLeftBorder( KWParagLayout::Border _leftBorder ) { leftBorder = _leftBorder; repaint( true ); }
	KWParagLayout::Border getRightBorder() { return rightBorder; }
	void setRightBorder( KWParagLayout::Border _rightBorder ) { rightBorder = _rightBorder; repaint( true ); }
	KWParagLayout::Border getTopBorder() { return topBorder; }
	void setTopBorder( KWParagLayout::Border _topBorder ) { topBorder = _topBorder; repaint( true ); }
	KWParagLayout::Border getBottomBorder() { return bottomBorder; }
	void setBottomBorder( KWParagLayout::Border _bottomBorder ) { bottomBorder = _bottomBorder; repaint( true ); }

protected:
	void drawContents( QPainter* ); 
	QPen setBorderPen( KWParagLayout::Border _brd ); 

	KWParagLayout::Border leftBorder, rightBorder, topBorder, bottomBorder; 

}; 

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

class KWNumPreview : public QGroupBox
{
	Q_OBJECT

public:
	KWNumPreview( QWidget*, const char* ); 
	~KWNumPreview() {}

	void setCounter( KWParagLayout::Counter _counter ) { counter = _counter; repaint( true ); }

protected:
	void drawContents( QPainter* ); 

	KWParagLayout::Counter counter; 

}; 

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

class KWParagDia : public QTabDialog
{
	Q_OBJECT

public:
	static const int PD_SPACING = 1; 
	static const int PD_FLOW = 2; 
	static const int PD_BORDERS = 4; 
	static const int PD_NUMBERING = 8; 
	static const int PD_TABS = 16; 

	KWParagDia( QWidget*, const char*, QStrList _fontList, int _flags, KWordDocument *_doc ); 
	~KWParagDia(); 

	int getFlags() { return flags; }

	void setLeftIndent( KWUnit _left ); 
	void setFirstLineIndent( KWUnit _first ); 
	void setSpaceAfterParag( KWUnit _after ); 
	void setSpaceBeforeParag( KWUnit _before ); 
	void setLineSpacing( KWUnit _spacing ); 

	void setFlow( KWParagLayout::Flow _flow ); 

	void setTabList( QList<KoTabulator> *tabList ); 

	KWUnit getLeftIndent(); 
	KWUnit getFirstLineIndent(); 
	KWUnit getSpaceBeforeParag(); 
	KWUnit getSpaceAfterParag(); 
	KWUnit getLineSpacing(); 

	KWParagLayout::Flow getFlow(); 

	KWParagLayout::Border getLeftBorder() { return leftBorder; }
	void setLeftBorder( KWParagLayout::Border _leftBorder ) { leftBorder = _leftBorder; updateBorders(); }
	KWParagLayout::Border getRightBorder() { return rightBorder; }
	void setRightBorder( KWParagLayout::Border _rightBorder ) { rightBorder = _rightBorder; updateBorders(); }
	KWParagLayout::Border getTopBorder() { return topBorder; }
	void setTopBorder( KWParagLayout::Border _topBorder ) { topBorder = _topBorder; updateBorders(); }
	KWParagLayout::Border getBottomBorder() { return bottomBorder; }
	void setBottomBorder( KWParagLayout::Border _bottomBorder ) { bottomBorder = _bottomBorder; updateBorders(); }

	void setCounter( KWParagLayout::Counter _counter ); 
	KWParagLayout::Counter getCounter() { return counter; }

protected:
	void setupTab1(); 
	void setupTab2(); 
	void setupTab3(); 
	void setupTab4(); 
	void setupTab5(); 
	void clearFlows(); 
	void updateBorders(); 

	QWidget *tab1, *tab2, *tab3, *tab4, *tab5; 
	QGridLayout *grid1, *grid2, *grid3, *grid4, *grid5, *indentGrid, *spacingGrid, *pSpaceGrid, *tgrid, *txtgrid, *ogrid, *tabGrid; 
	KRestrictedLine *eLeft, *eRight, *eFirstLine, *eSpacing, *eBefore, *eAfter, *eTabPos; 
	QLabel *lLeft, *lRight, *lFirstLine, *lBefore, *lAfter, *lFlow, *lStyle, *lWidth, *lColor, *lDepth, *lcLeft, *lcRight, *lStart; 
	QGroupBox *indentFrame, *spacingFrame, *pSpaceFrame, *gType, *gText, *gOther; 
	QComboBox *cSpacing, *cStyle, *cWidth; 
	QRadioButton *rLeft, *rCenter, *rRight, *rBlock, *rANums, *rLRNums, *rURNums, *rLAlph, *rUAlph, *rBullets, *rList, *rChapter, *rNone; 
	KWPagePreview *prev1; 
	KWPagePreview2 *prev2; 
	KButtonBox *bb, *bbTabs; 
	QPushButton *bLeft, *bRight, *bTop, *bBottom, *bBullets, *bFont, *bAdd, *bDel, *bModify; 
	KWBorderPreview *prev3; 
	KColorButton *bColor; 
	QSpinBox *sDepth; 
	QButtonGroup *g1, *g2, *g3; 
	KWNumPreview *prev4; 
	QLineEdit *ecLeft, *ecRight, *eStart; 
	QListBox *lTabs; 
	QLabel *lTab; 
	QRadioButton *rtLeft, *rtCenter, *rtRight, *rtDecimal; 


	KWParagLayout::Border leftBorder, rightBorder, topBorder, bottomBorder; 
	int flags; 
	KWParagLayout::Counter counter; 
	QStrList fontList; 
	KWordDocument *doc; 

protected slots:
	void leftChanged( const QString & ); 
	void rightChanged( const QString & ); 
	void firstChanged( const QString & ); 
	void spacingActivated( int ); 
	void spacingChanged( const QString & ); 
	void beforeChanged( const QString & ); 
	void afterChanged( const QString & ); 
	void flowLeft(); 
	void flowCenter(); 
	void flowRight(); 
	void flowBlock(); 
	void brdLeftToggled( bool ); 
	void brdRightToggled( bool ); 
	void brdTopToggled( bool ); 
	void brdBottomToggled( bool ); 
	void brdStyleChanged( const QString & ); 
	void brdWidthChanged( const QString & ); 
	void brdColorChanged( const QColor& ); 
	void changeBullet(); 
	void typeChanged( int ); 
	void numTypeChanged( int ); 
	void leftTextChanged( const QString & ); 
	void rightTextChanged( const QString & ); 
	void startChanged( const QString & ); 
	void depthChanged( int ); 

}; 

#endif


