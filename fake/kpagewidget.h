#ifndef FAKE_KPAGEWIDGET_H
#define FAKE_KPAGEWIDGET_H

#include <QTabWidget>
#include <kpagewidgetitem.h>

class KPageWidget : public QTabWidget
{
public:
    KPageWidget(QWidget *parent = 0) : QTabWidget(parent) {}
    void addPage(KPageWidgetItem *item) { m_tabWidget->addTab(item->widget(), item->header()); }
};

#endif
