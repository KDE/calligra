/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_VIEW
#define KCHART_VIEW

#include <koView.h>

class KAction;
class KToggleAction;
class QPaintEvent;

class KChartPart;

class DCOPObject;

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
      void edit();
      void slotConfig();
      void wizard();
      void saveConfig();
      void loadConfig();
      void defaultConfig();
      void createTempData();
      void pieChart();
      void barsChart();
      void lineChart();
      void areasChart();
      void hiLoChart();
      void ringChart();
      void slotRepaint();
      void slotConfigBack();
      void slotConfigFont();
      void slotConfigColor();

protected:
    void paintEvent( QPaintEvent* );

    virtual void updateReadWrite( bool readwrite );

    virtual void mousePressEvent ( QMouseEvent * );

private:
    KAction* m_cut;
    KAction* m_wizard;
    KAction* m_edit;
    KAction* m_config;
    KAction* m_saveconfig;
    KAction* m_loadconfig;
    KAction* m_defaultconfig;
    KAction *m_colorConfig;
    KAction* m_fontConfig;
    KAction* m_backConfig;
    KToggleAction* m_chartpie;
    KToggleAction* m_chartareas;
    KToggleAction* m_chartbars;
    KToggleAction* m_chartline;
    KToggleAction* m_charthilo;
    KToggleAction* m_chartring;

    DCOPObject *dcop;
};

#endif
