/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTFONTCONFIGPAGE_H__
#define __KCHARTFONTCONFIGPAGE_H__

#include <qwidget.h>

#include "kchartcolorarray.h"

class QLineEdit;
class QListBox;
class QPushButton;
class KColorButton;

class KDChartParams;

class KChartFontConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartFontConfigPage( KDChartParams* params,QWidget* parent );
    void init();
    void apply();
    void initList();

public slots:
    void changeIndex(int index);
    void changeLabelFont();
private:
    KDChartParams* _params;
    QLineEdit *font;
    QListBox *list;
    QListBox *listColor;
    QPushButton *fontButton;
    KColorButton *colorButton;
    QFont title;
    QFont xtitle;
    QFont ytitle;
    QFont label;
    QFont yaxis;
    QFont xaxis;
    KChartColorArray extColor;
    int index;
};
#endif
