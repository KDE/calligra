#ifndef KIS_WINTAB_TABLET_HANDLER_H
#define KIS_WINTAB_TABLET_HANDLER_H

#include <QObject>
#include <QWidget>

class KisWinTabTabletHandler
{
public:
    KisWinTabTabletHandler(QWidget *widget);
    virtual ~KisWinTabTabletHandler();

    bool winEvent(MSG *message, long *result);

private:

    QWidget *m_widget;

};

#endif // KIS_WINTAB_TABLET_HANDLER_H
