
#ifndef KPT_VIEW_IFACE_H
#define KPT_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>
#include <qrect.h>
#include <qcolor.h>

class KPTView;

class KPtViewIface : public KoViewIface
{
    K_DCOP
public:
    KPtViewIface( KPTView* );
    ~KPtViewIface();

k_dcop:
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewGantt();
    void slotViewPert();
    void slotViewResources();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotConfigure();

private:
    KPTView* m_view;
};

#endif
