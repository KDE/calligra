#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialog.h>   
#include "kchart_part.h"
#include "sheetdlg.h"  
#include <qstrlist.h>

class KDChartParams;

class kchartDataEditor : public KDialog 
{
    Q_OBJECT
public:
    kchartDataEditor();
    void setData(KoChart::Data* dat);
    void getData(KoChart::Data* dat);
    void getLegend(KDChartParams* params);
    void setLegend(const QStringList &legend);
    void getXLabel(KDChartParams* params);
    void setXLabel(const QStringList &xlbl);
private:
    SheetDlg *_widget;
};


#endif
