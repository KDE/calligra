#ifndef _KIVIO_PROTECTION_PANEL_H
#define _KIVIO_PROTECTION_PANEL_H

class KAction;
class KivioView;
class KivioLayer;

#include "kivio_protection_panel_base.h"

class KivioProtectionPanel : public KivioProtectionPanelBase
{
Q_OBJECT

public:
    KivioProtectionPanel( KivioView *view, QWidget *parent=0, const char *name=0 );
    virtual ~KivioProtectionPanel();

public slots:
   void updateCheckBoxes();
   void togWidth(bool);
   void togHeight(bool);
   void togAspect(bool);
   void togDelete(bool);
   void togX(bool);
   void togY(bool);

protected:
   KivioView *m_pView;
};

#endif
