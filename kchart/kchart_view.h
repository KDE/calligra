/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_VIEW
#define KCHART_VIEW


#include <KoView.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

//#include "KDChartAxisParams.h"


class KAction;
class KToggleAction;
class QPaintEvent;


namespace KChart
{

class KChartPart;
class ViewAdaptor;

class KChartView : public KoView
{
    Q_OBJECT
public:
    explicit KChartView( KChartPart* part, QWidget* parent = 0 );
    ~KChartView();

    void updateGuiTypeOfChart();
    virtual ViewAdaptor* dbusObject();
    void config(int flag);

public slots:
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
    KAction  *m_edit;
    KAction  *m_saveconfig;
    KAction  *m_loadconfig;
    KAction  *m_defaultconfig;

    KToggleAction  *m_chartpie;
    KToggleAction  *m_chartareas;
    KToggleAction  *m_chartbars;
    KToggleAction  *m_chartline;
    KToggleAction  *m_charthilo;
    KToggleAction  *m_chartring;
    KToggleAction  *m_chartpolar;
    KToggleAction  *m_chartbw;
    ViewAdaptor *m_dbus;

    // This is used for a workaround for a bug in the kdchart code, see #101490.
    bool m_logarithmicScale;
    void forceAxisParams(bool lineMode);
};

}  //KChart namespace

#endif
