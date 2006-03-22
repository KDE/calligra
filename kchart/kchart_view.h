/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_VIEW
#define KCHART_VIEW


#include <KoView.h>
#include "kdchart/KDChartAxisParams.h"

class KAction;
class KToggleAction;
class QPaintEvent;

class DCOPObject;

namespace KChart
{

class KChartPart;
class kchartDataEditor;

class KChartView : public KoView
{
    Q_OBJECT
public:
    KChartView( KChartPart* part, QWidget* parent = 0, const char* name = 0 );
    ~KChartView();

    void updateGuiTypeOfChart();
    virtual DCOPObject* dcopObject();

    void config(int flag);

public slots:
    void  editData();
     void applyEdit(kchartDataEditor *ed);
    void  slotConfig();
    void  wizard();
    void  saveConfig();
    void  loadConfig();
    void  defaultConfig();

    void  pieChart();
    void  barsChart();
    void  lineChart();
    void  areasChart();
    void  hiLoChart();
    void  ringChart();
    void  polarChart();
    void  bwChart();
    void  print(KPrinter &);
    void  setupPrinter(KPrinter &);

    void  slotRepaint();
    void  slotConfigBack();
    void  slotConfigFont();
    void  slotConfigColor();
    void  slotConfigLegend();
    void  slotConfigHeaderFooterChart();
    void  slotConfigSubTypeChart();
    void  slotConfigDataFormat();

    void  slotConfigPageLayout();
    void  importData();
    void  extraCreateTemplate();

protected:
    void          paintEvent( QPaintEvent* );

    virtual void  updateReadWrite( bool readwrite );

    virtual void  mousePressEvent ( QMouseEvent * );
    void          updateButton();

private:
    KAction  *m_importData;
    KAction  *m_wizard;
    KAction  *m_edit;
    KAction  *m_config;
    KAction  *m_saveconfig;
    KAction  *m_loadconfig;
    KAction  *m_defaultconfig;
    KAction  *m_colorConfig;
    KAction  *m_fontConfig;
    KAction  *m_backConfig;
    KAction  *m_legendConfig;
    KAction  *m_dataFormatConfig;
    KAction  *m_subTypeChartConfig;
    KAction  *m_headerFooterConfig;
    KAction  *m_pageLayoutConfig;

    KToggleAction  *m_chartpie;
    KToggleAction  *m_chartareas;
    KToggleAction  *m_chartbars;
    KToggleAction  *m_chartline;
    KToggleAction  *m_charthilo;
    KToggleAction  *m_chartring;
    KToggleAction  *m_chartpolar;
    KToggleAction  *m_chartbw;

    DCOPObject  *m_dcop;

    // This is used for a workaround for a bug in the kdchart code, see #101490.
    bool m_logarithmicScale;
    void forceAxisParams(bool lineMode);
};

}  //KChart namespace

#endif
