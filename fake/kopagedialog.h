#ifndef FAKE_KPAGEDIALOG_H
#define FAKE_KPAGEDIALOG_H

#include <QTabWidget>
#include <kdialog.h>
#include <kfakepagewidgetitem.h>

class KoPageDialog : public KDialog
{
public:
    enum FaceType { Tree, Tabbed, List, Plain };

    KoPageDialog(QWidget *parent = 0) : KDialog(parent) { m_tabWidget = new QTabWidget(this); }
    virtual ~KoPageDialog() { qDeleteAll(m_items); }

    void setFaceType(FaceType) {}

    KFakePageWidgetItem* addPage(QWidget *page, const QString &title) { return addPage(new KFakePageWidgetItem(page, title)); }
    KFakePageWidgetItem* addPage(KFakePageWidgetItem *item) {
        m_items.append(item);
        m_tabWidget->addTab(item->widget(), item->header());
        return item;
    }

    KFakePageWidgetItem* currentPage() const {
        QWidget *w = m_tabWidget->currentWidget();
        Q_FOREACH(KFakePageWidgetItem *item, m_items)
            if (item->widget() == w)
                return item;
        return 0;
    }
    void setCurrentPage(KFakePageWidgetItem *item) { m_tabWidget->setCurrentWidget(item ? item->widget() : 0); }

private:
    QTabWidget *m_tabWidget;
    QList<KFakePageWidgetItem *> m_items;
};

#endif
