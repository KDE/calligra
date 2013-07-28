#include "WikiClientDocker.h"
#include "WikiClientWidget.h"
#include "KWCanvas.h"

#include <klocale.h>
#include <QSpacerItem>
#include <QGridLayout>

class WikiClientDocker::Private
{
public:
    Private()
      : m_canvasReset(false)
    {}

    QSpacerItem *spacer;
    QGridLayout *layout;
    bool m_canvasReset;
    WikiClientWidget *m_wikiClientWidget;
};
WikiClientDocker::WikiClientDocker()
    : d(new Private())
{
    setWindowTitle(i18n("Wiki Client"));

    d->m_wikiClientWidget = new WikiClientWidget();

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(ondockLocationChanged(Qt::DockWidgetArea)));

    setWidget(d->m_wikiClientWidget);
}

WikiClientDocker::~WikiClientDocker()
{
}

void WikiClientDocker::ondockLocationChanged(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        d->m_wikiClientWidget->setLayoutDirection(WikiClientWidget::LayoutHorizontal);
    } else {
        d->m_wikiClientWidget->setLayoutDirection(WikiClientWidget::LayoutVertical);
    }
}
