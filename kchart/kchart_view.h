/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_VIEW
#define KCHART_VIEW

#include <container.h>

class QAction;
class QPaintEvent;

class KChartPart;

class KChartView : public ContainerView
{
    Q_OBJECT
public:
    KChartView( KChartPart* part, QWidget* parent = 0, const char* name = 0 );
    
protected slots:
      void edit();
      void wizard();
      void saveConfig();
      void loadConfig();
    
protected:
    void paintEvent( QPaintEvent* );
    
private:
    QAction* m_cut;
    QAction* m_wizard;
    QAction* m_edit;
    QAction* m_saveconfig;
    QAction* m_loadconfig;
    
};

#endif
