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
/* Module: Table Dialog (header)                                  */
/******************************************************************/

#ifndef tabledia_h
#define tabledia_h

#include <qtabdialog.h>
#include <qlist.h>

#include "paraglayout.h"
#include "format.h"

class KWPage;
class KWordDocument;
class QGridLayout;
class QLabel;
class QListBox;
class QWidget;
class QCheckBox;
class QButtonGroup;

/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

class KWTablePreview : public QWidget
{
    Q_OBJECT

public:
    KWTablePreview( QWidget *_parent, int _rows, int _cols )
        : QWidget( _parent ), rows( _rows ), cols( _cols ) {}

    void setRows( int _rows ) { rows = _rows; repaint( true ); }
    void setCols( int _cols ) { cols = _cols; repaint( true ); }

protected:
    void paintEvent( QPaintEvent *e );

    int rows, cols;

};

/******************************************************************/
/* Class: KWTableConf                                             */
/******************************************************************/

class KWTableConf : public QWidget
{
    Q_OBJECT

public:
    KWTableConf( QWidget *_parent, KWordDocument *_doc );

protected:
    struct TableStyle
    {
        bool hasHeader, hasFirstCol;

        KWParagLayout::Border hTop, hBottom, hRight, hLeft;
        QBrush hBack;
        KWParagLayout::Border frTop, frBottom, frRigfrt, frLeft;
        QBrush frBack;
        KWParagLayout::Border bTop, bBottom, bRigbt, bLeft;
        QBrush bBack;

        KWFormat header, firstRow, Body;
    };

    void readTableStyles();
    void setupPage();

    QGridLayout *grid1;
    QLabel *lStyles;
    QListBox *lbStyles;
    QWidget *preview;
    QCheckBox *cbHeaderOnAllPages;
    QButtonGroup *bgHeader, *bgFirstCol, *bgBody;
    QCheckBox *cbHBorder, *cbHBack, *cbHFormat;
    QCheckBox *cbFCBorder, *cbFCBack, *cbFCFormat;
    QCheckBox *cbBodyBorder, *cbBodyBack, *cbBodyFormat;

    KWordDocument *doc;
    QList<TableStyle> tableStyles;

};

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

class KWTableDia : public QTabDialog
{
    Q_OBJECT

public:
    KWTableDia( QWidget *parent, const char *name, KWPage *_page, KWordDocument *_doc, int rows, int cols );

protected:
    void setupTab1( int rows, int cols );
    void setupTab2();
    void closeEvent( QCloseEvent *e ) { emit cancelButtonPressed(); }

    QWidget *tab1;
    QGridLayout *grid1;
    QLabel *lRows, *lCols;
    QSpinBox *nRows, *nCols;
    KWTablePreview *preview;
    KWTableConf *tab2;

    KWPage *page;
    KWordDocument *doc;

protected slots:
    void insertTable();
    void rowsChanged( int );
    void colsChanged( int );

};

#endif


