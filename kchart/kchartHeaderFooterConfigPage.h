/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTHEADERFOOTERCONFIGPAGE_H__
#define __KCHARTHEADERFOOTERCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbutton.h>

class QLineEdit;
class QPushButton;

class KChartParams;

class KChartHeaderFooterConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartHeaderFooterConfigPage( KChartParams* params, QWidget* parent );
    void init();
    void apply();
protected slots:
    void changeTitleFont();
    void changeSubtitleFont();
    void changeFooterFont();
private:
    KChartParams* _params;

    QLineEdit *titleEdit;
    KColorButton *titleColorButton;
    QPushButton *titleFontButton;
    QFont titleFont;
    QButton::ToggleState titleFontIsRelative;
    
    QLineEdit *subtitleEdit;
    KColorButton *subtitleColorButton;	
    QPushButton *subtitleFontButton;
    QFont subtitleFont;
    QButton::ToggleState subtitleFontIsRelative;

    QLineEdit *footerEdit;
    KColorButton *footerColorButton;
    QPushButton *footerFontButton;
    QFont footerFont;
    QButton::ToggleState footerFontIsRelative;
};

#endif
