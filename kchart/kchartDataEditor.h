#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialog.h>   
#include "kchart_part.h"
#include "sheetdlg.h"  
#include <qstrlist.h>

class KChartParams;

class kchartDataEditor : public KDialog 
{
    Q_OBJECT
public:
    kchartDataEditor();
    kchartDataEditor(QWidget* parent);
    void setData(KoChart::Data* dat);
    void getData(KoChart::Data* dat);
    void getLegend(KChartParams* params);
    void setLegend(const QStringList &legend);
    void getXLabel(KChartParams* params);
    void setXLabel(const QStringList &xlbl);
    void setAxisLabelTextLong( QStringList *_longLabels ) { longLabels = _longLabels; }
    void setAxisLabelTextShort( QStringList *_shortLabels ){ shortLabels = _shortLabels; }

private:
    SheetDlg *_widget;
    QStringList *longLabels;
    QStringList *shortLabels;
};


#endif
