/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef FOOTER_HEADER_H
#define FOOTER_HEADER_H

#include <qvbox.h>
#include <ktoolbar.h>

class KPresenterDoc;
class KToolBar;

class QTabWidget;
class QCheckBox;
class QPushButton;
class QColor;
class QFont;

/******************************************************************/
/* Class: KPFooterHeaderEditor					  */
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

    void setShowHeader( bool b );
    void setShowFooter( bool b );

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

    int h_bold, h_italic, h_underline, h_color, h_aleft, h_acenter, h_aright, h_font, h_size, h_pgnum;
    int f_bold, f_italic, f_underline, f_color, f_aleft, f_acenter, f_aright, f_font, f_size, f_pgnum;

public slots:
    void slotCloseDia();

protected slots:
    void slotShowHeader();
    void slotShowFooter();
    void slotUpdatePage();
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
    void headerInsertPageNum();

    void footerFont( const QString &f );
    void footerSize( const QString &s );
    void footerBold();
    void footerItalic();
    void footerUnderline();
    void footerColor();
    void footerAlignLeft();
    void footerAlignCenter();
    void footerAlignRight();
    void footerInsertPageNum();

    void headerFontChanged( const QFont & );
    void headerColorChanged( const QColor &c );
    void headerAlignChanged( int ha );

    void footerFontChanged( const QFont &f );
    void footerColorChanged( const QColor &c );
    void footerAlignChanged( int ha );

    void tabSelected( const QString &s );

};

#endif
