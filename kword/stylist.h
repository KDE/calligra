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
/* Module: Stylist Dialog                                         */
/******************************************************************/

#ifndef stylist_h
#define stylist_h

#include <qtabdialog.h>

#include "paragdia.h"

class KWordDocument;
class KWStyleEditor;
class QWidget;
class QGridLayout;
class QListBox;
class QPushButton;
class KButtonBox;
class QComboBox;
class QLineEdit;
class QLabel;

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

class KWStyleManager : public QTabDialog
{
    Q_OBJECT

public:
    KWStyleManager( QWidget *_parent, KWordDocument *_doc, QStrList _fontList );

protected:
    void setupTab1();
    void setupTab2();

    QWidget *tab1, *tab2;
    QGridLayout *grid1, *grid2;
    QListBox *lStyleList;
    QPushButton *bEdit, *bDelete, *bAdd, *bUp, *bDown, *bCopy;
    KButtonBox *bButtonBox;
    QComboBox *cFont, *cColor, *cBorder, *cIndent, *cAlign, *cNumbering, *cTabs;

    KWordDocument *doc;
    KWStyleEditor *editor;
    QStrList fontList;

protected slots:
    void apply();
    void editStyle();
    void editStyle( int ) { editStyle(); }
    void addStyle();
    void deleteStyle();
    void updateStyleList();
    void updateButtons( const QString & );

};

/******************************************************************/
/* Class: KWStylePreview                                         */
/******************************************************************/

class KWStylePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWStylePreview( const char *title, QWidget *parent, KWParagLayout *_style ) : QGroupBox( title, parent, "" )
    { style = _style; }

protected:
    void drawContents( QPainter *painter );

    KWParagLayout *style;

};

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

class KWStyleEditor : public QTabDialog
{
    Q_OBJECT

public:
    KWStyleEditor( QWidget *_parent, KWParagLayout *_style, KWordDocument *_doc, QStrList _fontList );
    ~KWStyleEditor() { delete style; }

protected:
    void setupTab1();

    QWidget *tab1, *nwid;
    QGridLayout *grid1, *grid2;
    QPushButton *bFont, *bColor, *bSpacing, *bAlign, *bBorders, *bNumbering, *bTabulators;
    KButtonBox *bButtonBox;
    KWStylePreview *preview;
    QLabel *lName, *lFollowing;
    QLineEdit *eName;
    QComboBox *cFollowing;

    KWParagLayout *style, *ostyle;
    KWordDocument *doc;
    KWParagDia *paragDia;
    QStrList fontList;

signals:
    void updateStyleList();

protected slots:
    void changeFont();
    void changeColor();
    void changeSpacing();
    void changeAlign();
    void changeBorders();
    void changeNumbering();
    void changeTabulators();
    void paragDiaOk();
    void apply();
    void fplChanged( const QString & n ) { style->setFollowingParagLayout( QString( n ) ); }

};

#endif




