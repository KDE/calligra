/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPIECONFIGPAGE_H__
#define __KCHARTPIECONFIGPAGE_H__

#include <qwidget.h>
#include <qlistview.h>

class QSpinBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QFont;
class QRadioButton;

class KChartParams;

class KChartPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartPieConfigPage( KChartParams* params, QWidget* parent );
    void init();
    void apply();
    void initList();
    
public slots:
    void changeValue(int);
    void slotselected(QListViewItem *);
    
private:
    int col;
    KChartParams* _params;
    QSpinBox *dist;
    QSpinBox *column;
    QListView *list;
    QSpinBox *angle;
    QSpinBox *depth;
    QSpinBox *explose;
    QMemArray<int> value;
    int pos;
};

#endif
