#ifndef KDCHARTDESIGNEREXTENSION_H
#define KDCHARTDESIGNEREXTENSION_H

#include "kdchart_export.h"
#include <QObject>
#include <QList>

// QtDesigner module includes
#include <taskmenu.h>
#include <default_extensionfactory.h>

namespace KDChart {
  class Widget;
  class Chart;
}

class QAction;

// This is for extending the popupmenu on charts, to add "Edit chart properties..."
class KDCHART_PLUGIN_EXPORT KDChartBaseTaskMenu: public QObject, public QDesignerTaskMenuExtension
{
  Q_OBJECT
  Q_INTERFACES(QDesignerTaskMenuExtension)
  Q_DISABLE_COPY( KDChartBaseTaskMenu )
public:
    explicit KDChartBaseTaskMenu( QObject *parentW );
    ~KDChartBaseTaskMenu();

  /* \reimp */
  QAction *preferredEditAction() const;
  /* \reimp */
  QList<QAction*> taskActions() const;

protected:
    virtual void doEditChartProperties() = 0;

private slots:
  void editChartProperties();

private:
  QAction * mEditPropertiesAction;

};

class KDCHART_PLUGIN_EXPORT KDChartWidgetTaskMenu : public KDChartBaseTaskMenu
{
    public:
        KDChartWidgetTaskMenu( KDChart::Widget * chartWidget, QObject * parentW );
    protected:
        void doEditChartProperties();
    private:
        KDChart::Widget * mChart;
};

class KDCHART_PLUGIN_EXPORT KDChartChartTaskMenu : public KDChartBaseTaskMenu
{
    public:
        KDChartChartTaskMenu( KDChart::Chart * chart, QObject * parentW );
    protected:
        void doEditChartProperties();
    private:
        KDChart::Chart * mChart;
};

class KDCHART_PLUGIN_EXPORT KDChartWidgetTaskMenuFactory: public QExtensionFactory
{
    Q_OBJECT
    Q_DISABLE_COPY( KDChartWidgetTaskMenuFactory )
public:
    explicit KDChartWidgetTaskMenuFactory( QExtensionManager *extMgr = 0 );

protected:
    virtual QObject * createExtension( QObject *object, const QString &iid, QObject *parentW ) const;
};

class KDCHART_PLUGIN_EXPORT KDChartChartTaskMenuFactory: public QExtensionFactory
{
    Q_OBJECT
    Q_DISABLE_COPY( KDChartChartTaskMenuFactory )
public:
    explicit KDChartChartTaskMenuFactory( QExtensionManager *extMgr = 0 );

protected:
    virtual QObject * createExtension( QObject *object, const QString &iid, QObject *parentW ) const;
};


#endif /* KDCHARTDESIGNEREXTENSION_H */
