/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTCOLORCONFIGPAGE_H__
#define __KCHARTCOLORCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbtn.h>

// PENDING(kalle) Make this dynamic.
#define NUMDATACOLORS 6

class KChartColorConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartColorConfigPage( QWidget* parent );

/*     void setAccentColor( QColor color ); */
/*     QColor accentColor() const; */
/*     void setAxisLabelColor( QColor color ); */
/*     QColor axisLabelColor() const; */
/*     void setForegroundColor( QColor color ); */
/*     QColor foregroundColor() const; */
/*     void setBackgroundColor( QColor color ); */
/*     QColor backgroundColor() const; */
/*     void setTextColor( QColor color ); */
/*     QColor textColor() const; */
/*     void setLabelColor( QColor color ); */
/*     QColor labelColor() const; */
/*     void setDataColor( uint dataset, QColor color ); */
/*     QColor dataColor( uint dataset ) const; */

private:
    KColorButton* _accentCB;
    KColorButton* _axislabelCB;
    KColorButton* _foregroundCB;
    KColorButton* _backgroundCB;
    KColorButton* _textCB;
    KColorButton* _labelCB;
    KColorButton* _dataCB[NUMDATACOLORS];
};


/* inline void KChartColorConfigPage::setAccentColor( QColor color ) */
/* { */
/*     _accentCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::accentColor() const */
/* { */
/*     return _accentCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setAxisLabelColor( QColor color ) */
/* { */
/*     _axislabelCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::axisLabelColor() const */
/* { */
/*     return _axislabelCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setForegroundColor( QColor color ) */
/* { */
/*     _foregroundCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::foregroundColor() const */
/* { */
/*     return _foregroundCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setBackgroundColor( QColor color ) */
/* { */
/*     _backgroundCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::backgroundColor() const */
/* { */
/*     return _backgroundCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setTextColor( QColor color ) */
/* { */
/*     _textCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::textColor() const */
/* { */
/*     return _textCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setLabelColor( QColor color ) */
/* { */
/*     _labelCB->setColor( color ); */
/* } */

/* inline QColor KChartColorConfigPage::labelColor() const */
/* { */
/*     return _labelCB->color(); */
/* } */


/* inline void KChartColorConfigPage::setDataColor( uint dataset, */
/* 						 QColor color ) */
/* { */
/*     if( dataset >= NUMDATACOLORS ) */
/* 	return; */
/*     else */
/* 	_dataCB[ dataset ]->setColor( color ); */
/* } */


/* inline QColor KChartColorConfigPage::dataColor( uint dataset ) const */
/* { */
/*     if( dataset >= NUMDATACOLORS ) */
/* 	return _dataCB[0]->color(); */
/*     else */
/* 	return _dataCB[ dataset ]->color(); */
/* } */


#endif
