/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Footer/Header                                          */
/******************************************************************/

#ifndef FOOTER_HEADER_H
#define FOOTER_HEADER_H

#include <qvbox.h>
#include <qevent.h>

#include <ktoolbar.h>

#include "ktextobject.h"

class KPresenterDoc; 

class QTabWidget; 
class QCheckBox; 
class QPushButton; 
class QColor; 
class QFont; 

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

class KPFooterHeaderEditor : public QVBox
{
	Q_OBJECT

public:
	KPFooterHeaderEditor( KPresenterDoc *_doc ); 
	~KPFooterHeaderEditor(); 

	void allowClose()
	{ _allowClose = true; }

	void updateSizes(); 

protected:
	void setupHeader(); 
	void setupFooter(); 

	void resizeEvent( QResizeEvent *e ) {
		QVBox::resizeEvent( e ); 
		htool2->updateRects( true ); 
		ftool2->updateRects( true ); 
	}
	void closeEvent( QCloseEvent *e ) {
		// this dialog must not get closed!!
		if ( !_allowClose ) {
			hide(); 
			e->ignore(); 
		} else {
			QVBox::closeEvent( e ); 
		}
	}

	QTabWidget *tabwidget; 
	QCheckBox *showHeader, *showFooter; 
	QPushButton *updatePage, *closeDia, *penBrush1, *penBrush2; 
	KToolBar *htool2, *ftool2; 

	KPresenterDoc *doc; 
	bool _allowClose; 

	int h_bold, h_italic, h_underline, h_color, h_aleft, h_acenter, h_aright, h_font, h_size; 
	int f_bold, f_italic, f_underline, f_color, f_aleft, f_acenter, f_aright, f_font, f_size; 

protected slots:
	void slotShowHeader(); 
	void slotShowFooter(); 
	void slotUpdatePage(); 
	void slotCloseDia(); 
	void slotHeaderPenBrush(); 
	void slotFooterPenBrush(); 

	void headerFont( const QString &f ); 
	void headerSize( const QString &s ); 
	void headerBold(); 
	void headerItalic(); 
	void headerUnderline(); 
	void headerColor(); 
	void headerAlignLeft(); 
	void headerAlignCenter(); 
	void headerAlignRight(); 

	void footerFont( const QString &f ); 
	void footerSize( const QString &s ); 
	void footerBold(); 
	void footerItalic(); 
	void footerUnderline(); 
	void footerColor(); 
	void footerAlignLeft(); 
	void footerAlignCenter(); 
	void footerAlignRight(); 

	void headerFontChanged( QFont *f ); 
	void headerColorChanged( QColor *c ); 
	void headerAlignChanged( TxtParagraph::HorzAlign ha ); 

	void footerFontChanged( QFont *f ); 
	void footerColorChanged( QColor *c ); 
	void footerAlignChanged( TxtParagraph::HorzAlign ha ); 

}; 

#endif
