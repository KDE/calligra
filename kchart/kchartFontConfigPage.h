/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTFONTCONFIGPAGE_H__
#define __KCHARTFONTCONFIGPAGE_H__

#include <qwidget.h>
#include <qbutton.h>

#include "kchartcolorarray.h"
#include "kchartDataEditor.h"
class QLineEdit;
class QListBox;
class QPushButton;
class KColorButton;

class KChartParams;

class KChartFontConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartFontConfigPage( KChartParams* params,QWidget* parent, KoChart::Data *dat);
    void init();
    void apply();
    void initList();

public slots:
    void changeIndex(uint index);
    void changeLabelFont();
private:
    KChartParams* _params;
    QLineEdit *font;
    QListBox *list;
    QListBox *listColor;
    QPushButton *fontButton;
    KColorButton *colorButton;
    QFont title;
    QFont xTitle;
    QFont yTitle;
    QFont label;
    QFont yAxis;
    QFont xAxis;
    QFont legend;
    QButton::ToggleState titleIsRelative;
    QButton::ToggleState xTitleIsRelative;
    QButton::ToggleState yTitleIsRelative;
    QButton::ToggleState labelIsRelative;
    QButton::ToggleState yAxisIsRelative;
    QButton::ToggleState xAxisIsRelative;
    QButton::ToggleState legendIsRelative;
    KChartColorArray extColor;
    uint index;
    KoChart::Data *data;
};
#endif
