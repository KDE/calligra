#ifndef _KCWIZARDCHARTTYPEPAGE_H
#define _KCWIZARDCHARTTYPEPAGE_H


#include <QWidget>
#include <QPixmap>

#include <kvbox.h>

#include "kchart_params.h"

class QGridLayout;
class QPushButton;
class QButtonGroup;


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


class KCWizardCharttype : public QWidget
{
  Q_OBJECT

public:
  KCWizardCharttype( QWidget* parent, KChartPart* chart );

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

  QButtonGroup  *m_typeBG;
  QGridLayout   *m_layout;

  // column and row position (used by addButton() to know where to put
  // the new button)
  int            m_colPos;
  int            m_rowPos;

  KChartParams::ChartType  m_type;

};

}  //namespace KChart

#endif
