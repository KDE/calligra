#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialogbase.h>   
#include "kchart_part.h"
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

    // Old stuff, soon to be removed.
    void getLegend(KChartParams* params);
    void setLegend(const QStringList &legend);
    void getXLabel(KChartParams* params);
    void setXLabel(const QStringList &xlbl);

private:
    void  addDocs();

signals:
    void applyClicked(kchartDataEditor *ed);

protected slots:
    void  slotApply();
    void  setRows(int rows);
    void  setCols(int cols);

private:
    // Data Editor, Old version.  Soon to be removed.
#if 0
    SheetDlg *_widget;
#endif
    QStringList *longLabels;
    QStringList *shortLabels;

    // Data Editor, TNG
    QTable      *m_table;
    QLabel      *m_rowsLA;
    QSpinBox    *m_rowsSB;
    QLabel      *m_colsLA;
    QSpinBox    *m_colsSB;

    // This member is set to true if the user shrinks the data table,
    // and confirms this by clicking OK in a warning dialog.
    bool        m_userWantsToShrink;
};

}  //KChart namespace

#endif
