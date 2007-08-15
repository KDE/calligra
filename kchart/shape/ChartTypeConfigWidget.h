#ifndef _KCWIZARDCHARTTYPEPAGE_H
#define _KCWIZARDCHARTTYPEPAGE_H


#include <QWidget>
#include <QPixmap>

#include <kvbox.h>

#include "KoShapeConfigWidgetBase.h"

class QGridLayout;
class QPushButton;
class QButtonGroup;

class KoShape;

namespace KChart
{
class ChartShape;

// Contains a button with a text label below it
//
// FIXME: Move to kchartWizardSelectChartTypePage.cc

class KChartButton : public KVBox
{
  Q_OBJECT
  public:
    KChartButton(QWidget* parent, const QString &, const KIcon &);
    ~KChartButton();

    QPushButton *button() const { return m_button;}
  private:
    QPushButton *m_button;
};


class ChartTypeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT

public:
    ChartTypeConfigWidget();
    ~ChartTypeConfigWidget();

    void open( KoShape* shape );
    void save();
    KAction* createAction();

    /// reimplemented 
    virtual bool showOnShapeCreate() { return true; }

public slots:
    void apply();

private slots:
    void chartTypeSelected( int type );

signals:
    void chartChange(int);

private:
    void addButton(const QString &name, const QString &icon_name, int type);
    void incPos();

private:
    class Private;
    Private * const d;
};

}  //namespace KChart

#endif
