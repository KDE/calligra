/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTLEGENDCONFIGPAGE_H__
#define __KCHARTLEGENDCONFIGPAGE_H__

#include <qwidget.h>
#include <qbutton.h>
#include <qlayout.h>

class KChartParams;
class QRadioButton;
class QLineEdit;
class KColorButton;
class QPushButton;

class KChartLegendConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLegendConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();
public slots:
    void changeTitleLegendFont();
    void changeTextLegendFont();

private:
    KChartParams* _params;
    QPushButton *lTopLeft,    *lTop,     *lTopRight,
                *lLeft,       *noLegend, *lRight,
                *lBottomLeft, *lBottom,  *lBottomRight;
    QLineEdit *title;
    KColorButton *legendTitleColor, *legendTextColor;
    QFont titleLegend, textLegend;
    QPushButton *titleLegendFontButton, *textLegendFontButton;
    QButton::ToggleState titleLegendIsRelative, textLegendIsRelative;

    QPushButton* addButton( QGridLayout* layout, QButtonGroup* gb,
                            const QString &toolTipText, const QString &icon,
                            int posY, int posX );
};

#endif
