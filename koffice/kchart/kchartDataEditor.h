#ifndef KCHART_DATA_EDITOR_H
#define KCHART_DATA_EDITOR_H


#include <qstrlist.h>
#include <qspinbox.h>

#include <kdialogbase.h>

#include "kchart_part.h"

class QLabel;
class QSpinBox;
class QCheckBox;


namespace KChart
{


class KChartParams;


// This class inherits QSpinBox, but fixes a problem with it.
// Consider the following case:
//
// 1. The SpinBox for rows has the value 4.
// 2. The user enters the number 2 into it.
// 3. The user presses the little up arrow in the spinbox.
// 4. valueChanged(2) is emitted and 2 rows are removed.
// 5. valueChanged(3) is emitted and 1 row is added.
// 
// Now (at least) one row is removed that was never meant to be
// removed and data is lost.  This class fixes that by changing the
// semantics.  So instead of the behaviour of above, the
// valueChanged(2) is never emitted and instead of valueChanged(3),
// valueChanged(5) is emitted.
//


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
    // True if we should ignore the next value change (see above).
    bool  m_ignore;
};

// ----------------------------------------------------------------


// The reason for having a special Table class is to implement
// keyboard navigation in editing mode.
//

class kchartDataTable : public QTable
{
    Q_OBJECT
public:
    kchartDataTable(QWidget *);
    ~kchartDataTable();

protected:
    // handles keyboard navigation
    bool eventFilter( QObject *obj, QEvent *ev );
};


// ----------------------------------------------------------------


class kchartDataEditor : public KDialogBase
{
    Q_OBJECT
public:
    kchartDataEditor(QWidget* parent = 0);
    void setData(KChartParams *params, KDChartTableData *dat);
    void getData(KChartParams *params, KDChartTableData *dat);
    void setRowLabels(const QStringList &rowLabels);
    void getRowLabels(QStringList &rowLabels);
    void setColLabels(const QStringList &colLabels);
    void getColLabels(QStringList &colLabels);

    bool modified() const { return m_modified; }

protected:
    /**
    * Returns the number of rows used as headers (ie. containing labels for the X axis or a series name,
    * depending on the orientation of the data)
    */
    int headerRows();
    /**
    * Returns the number of columns used as headers (ie. containing labels for the X axis or a series name, 
    * depending on the orientation of the data)
    */
    int headerCols();

    /**
    * Updates the table widget's vertical header to match the row labels specified in the leftmost column of 
    * each row
    */
    void updateRowHeaders();

    /**
    * Updates the table widget's horizontal header to match the column labels specified in the top row of each
    * column
    */
    void updateColHeaders();

private:
    void  addDocs();

signals:
    void applyClicked(kchartDataEditor *ed);

private slots:
    void  slotApply();
    
    /** Removes the row which the current cell belongs to */
    void  removeCurrentRow();   
    /** Removes the column which the current cell belongs to */
    void  removeCurrentColumn();
    /** Inserts a new row below the current cell */
    void  insertRow();
    /** Inserts a new column to the right of the current cell */
    void  insertColumn();
    
    void  setRows(int rows);
    void  setCols(int cols);

    // Called when something changes in the table.
    void  tableChanged(int row, int col);

    // Called when the current cell is changed
    void  currentChanged(int row, int col);

private:
    // Widgets in the editor
    kchartDataTable    *m_table;

    QPushButton        *m_insertRowButton;
    QPushButton        *m_insertColButton;
    QPushButton        *m_removeRowButton;
    QPushButton        *m_removeColButton;

    QLabel             *m_rowsLA;
    kchartDataSpinBox  *m_rowsSB;
    QLabel             *m_colsLA;
    kchartDataSpinBox  *m_colsSB;
    QCheckBox          *m_firstRowAsLabel;
    QCheckBox          *m_firstColAsLabel;

    // This member is set to true if the user shrinks the data table,
    // and confirms this by clicking OK in a warning dialog.
    bool  m_userWantsToShrink;

    bool  m_modified;
};

}  //KChart namespace

#endif
