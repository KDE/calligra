#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H

#include <kdialogbase.h>
#include <qspinbox.h>
#include "kchart_part.h"
#include <qstrlist.h>

class QLabel;
class QSpinBox;

namespace KChart
{

class KChartParams;

class kchartDataSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    kchartDataSpinBox(QWidget *);
    ~kchartDataSpinBox();

public slots:
    // The user pressed the Up-button
    void stepUp();
    // The user pressed the Down-button
    void stepDown();
protected:
    void interpretText(){;};
    bool eventFilter( QObject *obj, QEvent *ev );

signals:
    // the value is changed (stepUp/stepDown was called or the focus is lost)
    void valueChangedSpecial(int);

private:
    bool m_ignore;
};


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

private:
    void  addDocs();

signals:
    void applyClicked(kchartDataEditor *ed);

private slots:
    void  slotApply();
    void  setRows(int rows);
    void  setCols(int cols);
    //the user clicked on a column in the column header
    void  column_clicked(int);
    //the user clicked on a row in the row header
    void  row_clicked(int);
    void test();

private:
    // Widgets in the editor
    QTable      *m_table;
    QLabel      *m_rowsLA;
    kchartDataSpinBox    *m_rowsSB;
    QLabel      *m_colsLA;
    kchartDataSpinBox    *m_colsSB;

    // This member is set to true if the user shrinks the data table,
    // and confirms this by clicking OK in a warning dialog.
    bool        m_userWantsToShrink;
};

}  //KChart namespace

#endif
