/*
 * $Id$
 *
 * Copyright 2000 by Matthias Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__
#define __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__

#include <qwidget.h>
#include <qpixmap.h>

class KDChartParams;
class QSpinBox;
class QCheckBox;
class QComboBox;
class QPixmap;

class KChartBackgroundPixmapConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartBackgroundPixmapConfigPage( KDChartParams* params,
                                      QWidget* parent );
    void init();
    void apply();

private slots:
    void setScaledToggled( bool b );
    void showSettings( const QString& ); 
    void slotBrowse();
    void slotWallPaperChanged( int );

private:
    void loadWallPaper();

    KDChartParams* _params;
    QComboBox* wallCB;
    QWidget* wallWidget;
    QSpinBox* intensitySB;
    QCheckBox* scaledCB;
    QCheckBox* centeredCB;
    QString wallFile;
    QPixmap wallPixmap;
};	

#endif
