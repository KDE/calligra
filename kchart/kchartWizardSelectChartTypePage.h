#ifndef _kchartWIZARDSELECTCHARTTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTTYPEPAGE_H

#include <QLayout>

#include <QWidget>
//Added by qt3to4:
#include <QPixmap>
#include <Q3GridLayout>
#include <kvbox.h>

#include "kchart_params.h"

class QPushButton;
class Q3ButtonGroup;

namespace KChart
{

class KChartPart;

// Contains a button with a text label below it
//
// FIXME: Move to kchartWizardSelectChartTypePage.cc
class KChartButton : public KVBox
{
  Q_OBJECT
  public:
    KChartButton(QWidget* parent, const QString &, const QPixmap &);
    ~KChartButton();

    QPushButton *button() const { return m_button;}
  private:
    QPushButton *m_button;
};

class KChartWizardSelectChartTypePage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSelectChartTypePage( QWidget* parent, KChartPart* chart );

public slots:
  void  apply();

private slots:
  void  chartTypeSelected( int type );

signals:
  void  chartChange(int);

private:
  void  addButton(const QString &name, const QString &icon_name, int type);
  void  incPos();

private:
  KChartPart    *m_chart;

  Q3ButtonGroup  *m_typeBG;
  Q3GridLayout   *m_layout;

  // column and row position (used by addButton() to know where to put
  // the new button)
  int            m_colPos;
  int            m_rowPos;

  KChartParams::ChartType  m_type;

};

}  //namespace KChart

#endif
