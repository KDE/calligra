#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialogbase.h>   
#include "kchart_part.h"
#include "sheetdlg.h"  
#include <qstrlist.h>

class QLabel;
class QSpinBox;

namespace KChart
{

class KChartParams;

class kchartDataEditor : public KDialogBase
{
    Q_OBJECT
public:
    kchartDataEditor(QWidget* parent = 0);
    void setData(KoChart::Data* dat);
    void getData(KoChart::Data* dat);
    void setRowLabels(const QStringList &rowLabels);
    void getRowLabels(QStringList &rowLabels);
    void setColLabels(const QStringList &colLabels);
    void getColLabels(QStringList &colLabels);

    void getLegend(KChartParams* params);
    void setLegend(const QStringList &legend);
    void getXLabel(KChartParams* params);
    void setXLabel(const QStringList &xlbl);

#if 0
    void setAxisLabelTextLong( QStringList *_longLabels ) { longLabels = _longLabels; }
    void setAxisLabelTextShort( QStringList *_shortLabels ){ shortLabels = _shortLabels; }
#endif

signals:
    void applyClicked(kchartDataEditor *ed);

protected slots:
    void  slotApply();
    void  setRows(int rows);
    void  setCols(int cols);

private:
    // Data Editor, Old version
    SheetDlg *_widget;
    QStringList *longLabels;
    QStringList *shortLabels;

    // Data Editor, TNG
    QTable      *m_table;
    QLabel      *m_rowsLA;
    QSpinBox    *m_rowsSB;
    QLabel      *m_colsLA;
    QSpinBox    *m_colsSB;
};

}  //KChart namespace

#endif
