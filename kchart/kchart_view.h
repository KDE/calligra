/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_VIEW
#define KCHART_VIEW

#include <koView.h>

class QAction;
class QPaintEvent;

class KChartPart;

class KChartView : public KoView
{
    Q_OBJECT
public:
    KChartView( KChartPart* part, QWidget* parent = 0, const char* name = 0 );

protected slots:
      void edit();
      void config();
      void wizard();
      void saveConfig();
      void loadConfig();
      void defaultConfig();
      void createTempData();
      void pieChart();
      void barsChart();
      void lineChart();
      void areasChart();
protected:
    void paintEvent( QPaintEvent* );

    virtual void updateReadWrite( bool readwrite );

private:
    QAction* m_cut;
    QAction* m_wizard;
    QAction* m_edit;
    QAction* m_config;
    QAction* m_saveconfig;
    QAction* m_loadconfig;
    QAction* m_defaultconfig;
    QAction* m_chartpie;
    QAction* m_chartareas;
    QAction* m_chartbars;
    QAction* m_chartline;
};

#endif
