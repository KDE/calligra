#include "kdchartextrainfoextension.h"

#include <QObject>
#include <QDebug>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesignerUI4>

#include <KDChartChart>
#include <KDChartWidget>
#include <KDChartWidgetSerializer.h>
#include <KDChartChartSerializer.h>

using namespace KDChart;

KDChartExtraInfoExtension::KDChartExtraInfoExtension(Chart *chart, QDesignerFormEditorInterface *desCore, QObject *parentObj)
  : QObject(parentObj), QDesignerExtraInfoExtension(),
    mChart(chart), mCore(desCore)
{
}

bool KDChartExtraInfoExtension::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    ChartSerializer s;
    return s.saveExtraInfo( mChart, ui_widget );
}

bool KDChartExtraInfoExtension::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    ChartSerializer s;
    return s.loadExtraInfo( mChart, ui_widget );
}

QWidget * KDChartExtraInfoExtension::widget() const { return mChart; }
QDesignerFormEditorInterface * KDChartExtraInfoExtension::core() const { return mCore; }

////

KDChartWidgetExtraInfoExtension::KDChartWidgetExtraInfoExtension(Widget *widget, QDesignerFormEditorInterface *desCore, QObject *parentObj)
    : QObject(parentObj), QDesignerExtraInfoExtension(),
    mWidget(widget), mCore(desCore)
{
}

bool KDChartWidgetExtraInfoExtension::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    WidgetSerializer s;
    return s.saveExtraInfo( mWidget, ui_widget );
}

bool KDChartWidgetExtraInfoExtension::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    WidgetSerializer s;
    return s.loadExtraInfo( mWidget, ui_widget );
}

////

KDChartExtraInfoExtensionFactory::KDChartExtraInfoExtensionFactory(QDesignerFormEditorInterface *core, QExtensionManager *parentObj )
  : QExtensionFactory( parentObj ), mCore( core )
{
}

QObject * KDChartExtraInfoExtensionFactory::createExtension(QObject *object, const QString &iid, QObject *parentObj) const
{
  if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
    return 0;

  if (Chart *w = qobject_cast<Chart*>(object))
    return new KDChartExtraInfoExtension( w, mCore, parentObj );

  if (Widget *w = qobject_cast<Widget*>(object))
      return new KDChartWidgetExtraInfoExtension( w, mCore, parentObj );

  return 0;
}

QWidget * KDChartWidgetExtraInfoExtension::widget() const { return mWidget; }
QDesignerFormEditorInterface * KDChartWidgetExtraInfoExtension::core() const { return mCore; }
