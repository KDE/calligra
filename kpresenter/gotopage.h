/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Dialog for skipping to a page during a presentation    */
/******************************************************************/

#ifndef gotopage_h
#define gotopage_h

#include <qframe.h>

class QWidget;
class QObject;
class QEvent;
class QResizeEvent;
class QSpinBox;
class QLabel;

/******************************************************************/
/* class KPGotoPage                                               */
/******************************************************************/

class KPGotoPage : public QFrame
{
    Q_OBJECT

public:
    KPGotoPage( int minValue, int maxValue, int start,
                QWidget *parent = 0L, const char *name = 0L, WFlags f = 0 );

    bool eventFilter( QObject *obj, QEvent *e );

    int getPage() { return page; }

    static int gotoPage( int minValue, int maxValue, int start, QWidget *parent = 0L );

protected:
    void resizeEvent( QResizeEvent *e );

    QSpinBox *spinbox;
    QLabel *label;

    int _default, page;

};


#endif
