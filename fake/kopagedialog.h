#ifndef FAKE_KPAGEDIALOG_H
#define FAKE_KPAGEDIALOG_H

#include <QTabWidget>
#include <kdialog.h>
#include <kpagewidgetitem.h>

class KoPageDialog : public KDialog
{
public:
    enum FaceType { Tree, Tabbed, List, Plain };

    KoPageDialog(QWidget *parent = 0) : KDialog(parent) { m_tabWidget = new QTabWidget(this); }
    virtual ~KoPageDialog() { qDeleteAll(m_items); }

    void setFaceType(FaceType) {}

    KPageWidgetItem* addPage(QWidget *page, const QString &title) { return addPage(new KPageWidgetItem(page, title)); }
    KPageWidgetItem* addPage(KPageWidgetItem *item) {
        m_items.append(item);
        m_tabWidget->addTab(item->widget(), item->header());
        return item;
    }

    KPageWidgetItem* currentPage() const {
        QWidget *w = m_tabWidget->currentWidget();
        Q_FOREACH(KPageWidgetItem *item, m_items)
            if (item->widget() == w)
                return item;
        return 0;
    }
    void setCurrentPage(KPageWidgetItem *item) { m_tabWidget->setCurrentWidget(item ? item->widget() : 0); }

private:
    QTabWidget *m_tabWidget;
    QList<KPageWidgetItem *> m_items;
};

#endif
