#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialog.h>   
#include "kchart_part.h"
#include "sheetdlg.h"  
#include <qstrlist.h>
#include "kchartparams.h"

class kchartDataEditor : public KDialog {
   Q_OBJECT
     public:
     kchartDataEditor();
   void setData(KChartData* dat);
   void getData(KChartData* dat);
   void getLabel(KChartParameters* params);
   void setLabel(QStringList lbl);
 private:
   SheetDlg *_widget;
};


#endif
