/*
 * Copyright 2000 by Matthias Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__
#define __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__

#include <qwidget.h>
#include <qpixmap.h>

class KChartParams;
class QSpinBox;
class QRadioButton;
class QComboBox;
class QPixmap;
class QListBox;
class KColorButton;

class KChartBackgroundPixmapConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartBackgroundPixmapConfigPage( KChartParams* params,
                                      QWidget* parent );
    void init();
    void apply();

private slots:
    void showSettings( const QString& ); 
    void slotBrowse();
    void slotWallPaperChanged( int );

private:
    void loadWallPaper();

    KChartParams* _params;
    QComboBox* wallCB;
    QWidget* wallWidget;
    QSpinBox* intensitySB;
    QRadioButton* stretchedRB;
    QRadioButton* scaledRB;
    QRadioButton* centeredRB;
    QRadioButton* tiledRB;
    QString wallFile;
    QPixmap wallPixmap;
    QListBox *regionList;
    KColorButton* _backgroundCB;	
};	

#endif
