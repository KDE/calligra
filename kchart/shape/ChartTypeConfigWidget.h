
#ifndef KCHART_CHART_TYPE_CONFIG_WIDGET
#define KCHART_CHART_TYPE_CONFIG_WIDGET

#include "KoShapeConfigWidgetBase.h"

class KoShape;

namespace KChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
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

private Q_SLOTS:
    void chartTypeSelected( int type );

Q_SIGNALS:
    void chartChange(int);

private:
    class Private;
    Private * const d;
};

}  // namespace KChart

#endif // KCHART_CHART_TYPE_CONFIG_WIDGET
