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
/* Module: Config Pie Dialog (header)                             */
/******************************************************************/

#ifndef confpiedia_h
#define confpiedia_h

#include <qdialog.h>
#include <qframe.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcombobox.h>
#include <qlineedit.h>

#include "global.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class QLabel;
class QGroupBox;
class QPushButton;
class QComboBox;
class QPainter;

/******************************************************************/
/* class PiePreview                                               */
/******************************************************************/

class PiePreview : public QFrame
{
    Q_OBJECT

public:
    PiePreview( QWidget* parent, const char* );
    ~PiePreview() {}

    void setAngle( int _angle )
    { angle = _angle; repaint( true ); }
    void setLength( int _len )
    { len = _len; repaint( true ); }
    void setType( PieType _type )
    { type = _type; repaint( true ); }
    void setPenBrush( QPen _pen, QBrush _brush )
    { pen = _pen; brush = _brush; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int angle, len;
    PieType type;
    QPen pen;
    QBrush brush;

};

/******************************************************************/
/* class ConfPieDia                                               */
/******************************************************************/

class ConfPieDia : public QDialog
{
    Q_OBJECT

public:
    ConfPieDia( QWidget* parent, const char* );
    ~ConfPieDia();

    void setAngle( int _angle )
    { angle = _angle; QString str; str.sprintf( "%d", _angle ); eAngle->setText( str ); piePreview->setAngle( angle ); }
    void setLength( int _len )
    { len = _len; QString str; str.sprintf( "%d", _len ); eLen->setText( str ); piePreview->setLength( len ); }
    void setType( PieType _type )
    { type = _type; cType->setCurrentItem( _type ); piePreview->setType( type ); }
    void setPenBrush( QPen _pen, QBrush _brush )
    { pen = _pen; brush = _brush; piePreview->setPenBrush( pen, brush );  }

    int getAngle()
    { return angle; }
    int getLength()
    { return len; }
    PieType getType()
    { return type; }

protected:
    QLabel *lType, *lAngle, *lLen;
    QLineEdit *eAngle, *eLen;
    QGroupBox *gSettings, *gPreview;
    PiePreview *piePreview;
    QPushButton *okBut, *applyBut, *cancelBut;
    QComboBox *cType;

    int angle, len;
    PieType type;
    QPen pen;
    QBrush brush;

protected slots:
    void lengthChanged( const QString & );
    void angleChanged( const QString & );
    void typeChanged( int );
    void Apply() { emit confPieDiaOk(); }

signals:
    void confPieDiaOk();

};

#endif


