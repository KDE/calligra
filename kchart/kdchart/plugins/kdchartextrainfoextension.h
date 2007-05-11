#ifndef KDCHARTEXTRAINFOEXTENSION_H
#define KDCHARTEXTRAINFOEXTENSION_H

#include "kdchart_export.h"

#include <QtDesigner/QDesignerExtraInfoExtension>
#include <QtDesigner/QExtensionFactory>

#include <qpointer.h>

class QDesignerFormEditorInterface;
namespace KDChart {
  class Chart;
  class Widget;
}
#if QT_VERSION < 0x040300
// Workaround Qt typo; fixed in 4.3
class DomUi;
#define DomUI DomUi
#endif

class KDCHART_PLUGIN_EXPORT KDChartExtraInfoExtension : public QObject, public QDesignerExtraInfoExtension
{
  Q_OBJECT
  Q_INTERFACES(QDesignerExtraInfoExtension)
  Q_DISABLE_COPY( KDChartExtraInfoExtension )
public:
    KDChartExtraInfoExtension( KDChart::Chart *chart, QDesignerFormEditorInterface *core, QObject *parent);

  virtual QWidget *widget() const;
  virtual QDesignerFormEditorInterface *core() const;

  virtual bool saveUiExtraInfo(DomUI *) { return false; }
  virtual bool loadUiExtraInfo(DomUI *) { return false; }

  virtual bool saveWidgetExtraInfo(DomWidget *ui_widget);
  virtual bool loadWidgetExtraInfo(DomWidget *ui_widget);
private:
  QPointer<KDChart::Chart> mChart;
  QPointer<QDesignerFormEditorInterface> mCore;
};

class KDCHART_PLUGIN_EXPORT KDChartWidgetExtraInfoExtension : public QObject, public QDesignerExtraInfoExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerExtraInfoExtension)
    Q_DISABLE_COPY( KDChartWidgetExtraInfoExtension )
    public:
        KDChartWidgetExtraInfoExtension( KDChart::Widget *w,
                                         QDesignerFormEditorInterface *core, QObject *parent);

        virtual QWidget *widget() const;
        virtual QDesignerFormEditorInterface *core() const;

        virtual bool saveUiExtraInfo(DomUI *) { return false; }
        virtual bool loadUiExtraInfo(DomUI *) { return false; }

        virtual bool saveWidgetExtraInfo(DomWidget *ui_widget);
        virtual bool loadWidgetExtraInfo(DomWidget *ui_widget);
    private:
        QPointer<KDChart::Widget> mWidget;
        QPointer<QDesignerFormEditorInterface> mCore;
};

class KDCHART_PLUGIN_EXPORT KDChartExtraInfoExtensionFactory: public QExtensionFactory
{
  Q_OBJECT
  Q_DISABLE_COPY( KDChartExtraInfoExtensionFactory )
public:
  explicit KDChartExtraInfoExtensionFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent = 0);

protected:
  virtual QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;

private:
  QDesignerFormEditorInterface *mCore;
};

#endif /* KDCHARTEXTRAINFOEXTENSION_H */
