#ifndef KSPREAD_VIEW_IFACE_H
#define KSPREAD_VIEW_IFACE_H

#include <KoViewIface.h>           

#include <qstring.h>
#include <qrect.h>

class KSpreadView;
class KSpreadCellProxy;

class KSpreadViewIface : public KoViewIface
{
    K_DCOP
public:
    KSpreadViewIface( KSpreadView* );
    ~KSpreadViewIface();

k_dcop:
    virtual DCOPRef doc() const;
    virtual DCOPRef map() const;
    virtual DCOPRef table() const;
    virtual void hide();
    virtual void show();

private:
    KSpreadView* m_view;
    KSpreadCellProxy* m_proxy;
};

#endif
