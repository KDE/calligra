#ifndef KPRANIMATIONSDOCKER_H
#define KPRANIMATIONSDOCKER_H

#include <QDockWidget>
#include <QMap>

#include <klocale.h>

class QListWidget;
class KPrView;

class KPrAnimationsDocker: public QDockWidget
{
    Q_OBJECT
public:
    KPrAnimationsDocker(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    void setView(KPrView* view);

public slots:


private:
    KPrView* m_view;
    QListWidget * m_layoutsView;
};

#endif // KPRANIMATIONSDOCKER_H
