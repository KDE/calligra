#ifndef __KDCHARTWIDGET_DESIGNER_CUSTOM_EDITOR_H__
#define __KDCHARTWIDGET_DESIGNER_CUSTOM_EDITOR_H__

#include "kdchart_export.h"

#include <QDialog>

#include "ui_kdchartwidgetdesignercustomeditor.h"

namespace KDChart {
    class Widget;
    class LegendPropertiesWidget;
    class AxisPropertiesWidget;
    class HeaderFooterPropertiesWidget;
}

class KDCHART_PLUGIN_EXPORT KDChartWidgetDesignerCustomEditor
    : public QDialog, private Ui::KDChartWidgetProperties
{
    Q_OBJECT
    public:
        KDChartWidgetDesignerCustomEditor( KDChart::Widget *chart );

    public slots:
        void slotTypeChanged( int index );
        void slotSubTypeChanged( int index );
        void slotLeadingTopChanged( int v );
        void slotLeadingLeftChanged( int v );
        void slotLeadingRightChanged( int v );
        void slotLeadingBottomChanged( int v );


    private slots:
        void slotAddLegend();
        void slotRemoveLegend();
        void slotCurrentLegendChanged( int idx );
        void slotAddAxis();
        void slotRemoveAxis();
        void slotCurrentAxisChanged( int idx );
        void slotAddHeaderFooter();
        void slotRemoveHeaderFooter();
        void slotCurrentHeaderFooterChanged( int idx );

    private:
        void setupLegendsTab();
        void setupAxesTab();
        void setupHeaderFooterTab();

        KDChart::LegendPropertiesWidget * mLegendEditor;
        KDChart::AxisPropertiesWidget * mAxisEditor;
        KDChart::HeaderFooterPropertiesWidget * mHeaderFooterEditor;
        KDChart::Widget * const mChart;

};

#endif
