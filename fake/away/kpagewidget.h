#ifndef FAKE_KPAGEWIDGET_H
#define FAKE_KPAGEWIDGET_H

#include <QTabWidget>
#include <kfakepagewidgetitem.h>

class KPageWidget : public QTabWidget
{
public:
    KPageWidget(QWidget *parent = 0) : QTabWidget(parent) {}
    void addPage(KFakePageWidgetItem *item) { m_tabWidget->addTab(item->widget(), item->header()); }
};

#endif
