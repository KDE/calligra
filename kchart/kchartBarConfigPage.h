/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTBARCONFIGPAGE_H__
#define __KCHARTBARCONFIGPAGE_H__

#include <qwidget.h>
#include "kchartparams.h"

class QRadioButton;
class QLineEdit;

class KChartBarConfigPage : public QWidget
{
    Q_OBJECT

	public:
    KChartBarConfigPage( QWidget* parent );

/*     void setOverwriteMode( OverwriteMode overwrite ); */
/*     OverwriteMode overwriteMode() const; */
/*     void setXAxisDistance( int xbardist ); */
/*     int xAxisDistance() const; */

 private:
    QRadioButton* _sidebysideRB;
    QRadioButton* _ontopRB;
    QRadioButton* _infrontRB;
    QLineEdit* _xbardist;
};


/* inline void KChartBarConfigPage::setXAxisDistance( int xbardist ) */
/* { */
/*     _xbardist->setText( QString().setNum( xbardist ) ); */
/* } */

/* inline int KChartBarConfigPage::xAxisDistance() const */
/* { */
/*     QString num = _xbardist->text(); */
/*     return num.toInt(); */
/* } */




#endif
