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
/* Module: Config Rect Dialog (header)                            */
/******************************************************************/

#ifndef confrectdia_h
#define confrectdia_h

#include <qdialog.h>
#include <qframe.h>

#include "global.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class QPainter;
class QLabel;
class QLineEdit;
class QGroupBox;
class QPushButton;

/******************************************************************/
/* class RectPreview                                               */
/******************************************************************/

class RectPreview : public QFrame
{
    Q_OBJECT

public:
    RectPreview( QWidget* parent, const char* );
    ~RectPreview() {}

    void setRnds( int _rx, int _ry )
    { xRnd = _rx; yRnd = _ry; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int xRnd, yRnd;

};

/******************************************************************/
/* class ConfRectDia                                              */
/******************************************************************/

class ConfRectDia : public QDialog
{
    Q_OBJECT

public:
    ConfRectDia( QWidget* parent, const char* );
    ~ConfRectDia();

    void setRnds( int _rx, int _rx );

    int getRndX()
    { return xRnd; }
    int getRndY()
    { return yRnd; }

protected:
    QLabel *lRndX, *lRndY;
    QLineEdit *eRndX, *eRndY;
    QGroupBox *gSettings, *gPreview;
    RectPreview *rectPreview;
    QPushButton *okBut, *applyBut, *cancelBut;

    int xRnd, yRnd;

protected slots:
    void rndXChanged( const QString & );
    void rndYChanged( const QString & );
    void Apply() { emit confRectDiaOk(); }

signals:
    void confRectDiaOk();

};

#endif


