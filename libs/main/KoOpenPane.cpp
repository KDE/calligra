/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoOpenPane.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QLinearGradient>
#include <QMimeData>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUrl>

#include <KLocalizedString>
#include <MainDebug.h>

#include "KoDetailsPane.h"
#include "KoRecentDocumentsPane.h"
#include "KoTemplate.h"
#include "KoTemplateGroup.h"
#include "KoTemplateTree.h"
#include "KoTemplatesPane.h"
#include "ui_KoOpenPaneBase.h"
#include <KoFileDialog.h>
#include <KoIcon.h>

#include <KConfigGroup>
#include <KSharedConfig>
#include <limits.h>

class KoSectionListItem : public QTreeWidgetItem
{
public:
    KoSectionListItem(QTreeWidget *treeWidget, const QString &name, int sortWeight, int widgetIndex = -1)
        : QTreeWidgetItem(treeWidget, QStringList() << name)
        , m_sortWeight(sortWeight)
        , m_widgetIndex(widgetIndex)
    {
        Qt::ItemFlags newFlags = Qt::NoItemFlags;

        if (m_widgetIndex >= 0)
            newFlags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        setFlags(newFlags);
    }

    bool operator<(const QTreeWidgetItem &other) const override
    {
        const KoSectionListItem *item = dynamic_cast<const KoSectionListItem *>(&other);

        if (!item)
            return 0;

        return ((item->sortWeight() - sortWeight()) < 0);
    }

    int sortWeight() const
    {
        return m_sortWeight;
    }

    int widgetIndex() const
    {
        return m_widgetIndex;
    }

private:
    int m_sortWeight;
    int m_widgetIndex;
};

class KoOpenPanePrivate : public Ui_KoOpenPaneBase
{
public:
    KoOpenPanePrivate()
        : Ui_KoOpenPaneBase()
    {
    }

    int m_freeCustomWidgetIndex;
};

KoOpenPane::KoOpenPane(QWidget *parent, const QStringList &mimeFilter, const QString &templatesResourcePath)
    : QWidget(parent)
    , d(new KoOpenPanePrivate)
{
    d->setupUi(this);

    m_mimeFilter = mimeFilter;
    d->m_openExistingButton->setText(i18n("Open Existing Document"));

    connect(d->m_openExistingButton, &QAbstractButton::clicked, this, &KoOpenPane::openFileDialog);

    connect(d->m_sectionList, &QTreeWidget::itemSelectionChanged, this, &KoOpenPane::updateSelectedWidget);
    connect(d->m_sectionList, &QTreeWidget::itemClicked, this, &KoOpenPane::itemClicked);
    connect(d->m_sectionList, &QTreeWidget::itemActivated, this, &KoOpenPane::itemClicked);

    initRecentDocs();
    initTemplates(templatesResourcePath);

    d->m_freeCustomWidgetIndex = 4;

    if (!d->m_sectionList->selectedItems().isEmpty()) {
        KoSectionListItem *selectedItem = static_cast<KoSectionListItem *>(d->m_sectionList->selectedItems().first());

        if (selectedItem) {
            d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
        }
    }

    QList<int> sizes;

    // Set the sizes of the details pane splitters
    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
    sizes = cfgGrp.readEntry("DetailsPaneSplitterSizes", sizes);

    if (!sizes.isEmpty())
        Q_EMIT splitterResized(nullptr, sizes);

    connect(this, &KoOpenPane::splitterResized, this, &KoOpenPane::saveSplitterSizes);

    setAcceptDrops(true);
}

KoOpenPane::~KoOpenPane()
{
    if (!d->m_sectionList->selectedItems().isEmpty()) {
        KoSectionListItem *item = dynamic_cast<KoSectionListItem *>(d->m_sectionList->selectedItems().first());

        if (item) {
            if (!qobject_cast<KoDetailsPane *>(d->m_widgetStack->widget(item->widgetIndex()))) {
                KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
                cfgGrp.writeEntry("LastReturnType", item->text(0));
            }
        }
    }

    delete d;
}

void KoOpenPane::openFileDialog()
{
    KoFileDialog dialog(this, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Open Existing Document"));
    dialog.setDefaultDir(qApp->applicationName().contains("karbon") ? QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                                                                    : QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog.setMimeTypeFilters(m_mimeFilter);
    dialog.setHideNameFilterDetailsOption();
    QUrl url = QUrl::fromUserInput(dialog.filename());
    Q_EMIT openExistingFile(url);
}

void KoOpenPane::initRecentDocs()
{
    QString header = i18n("Recent Documents");
    KoRecentDocumentsPane *recentDocPane = new KoRecentDocumentsPane(this, header);
    connect(recentDocPane, &KoDetailsPane::openUrl, this, &KoOpenPane::openExistingFile);
    QTreeWidgetItem *item = addPane(header, koIconName("document-open"), recentDocPane, 0);
    connect(recentDocPane, &KoDetailsPane::splitterResized, this, &KoOpenPane::splitterResized);
    connect(this, &KoOpenPane::splitterResized, recentDocPane, &KoDetailsPane::resizeSplitter);

    if (KSharedConfig::openConfig()->hasGroup("RecentFiles")) {
        d->m_sectionList->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
    }
}

void KoOpenPane::initTemplates(const QString &templatesResourcePath)
{
    QTreeWidgetItem *selectItem = nullptr;
    QTreeWidgetItem *firstItem = nullptr;
    const int templateOffset = 1000;

    if (!templatesResourcePath.isEmpty()) {
        KoTemplateTree templateTree(templatesResourcePath, true);

        foreach (KoTemplateGroup *group, templateTree.groups()) {
            if (group->isHidden()) {
                continue;
            }

            KoTemplatesPane *pane = new KoTemplatesPane(this, group->name(), group, templateTree.defaultTemplate());
            connect(pane, &KoDetailsPane::openUrl, this, &KoOpenPane::openTemplate);
            connect(pane, &KoTemplatesPane::alwaysUseChanged, this, &KoOpenPane::alwaysUseChanged);
            connect(this, &KoOpenPane::alwaysUseChanged, pane, &KoTemplatesPane::changeAlwaysUseTemplate);
            connect(pane, &KoDetailsPane::splitterResized, this, &KoOpenPane::splitterResized);
            connect(this, &KoOpenPane::splitterResized, pane, &KoDetailsPane::resizeSplitter);
            QTreeWidgetItem *item = addPane(group->name(), group->templates().first()->loadPicture(), pane, group->sortingWeight() + templateOffset);

            if (!firstItem) {
                firstItem = item;
            }

            if (group == templateTree.defaultGroup()) {
                firstItem = item;
            }

            if (pane->isSelected()) {
                selectItem = item;
            }
        }
    } else {
        firstItem = d->m_sectionList->topLevelItem(0);
    }

    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");

    if (selectItem && (cfgGrp.readEntry("LastReturnType") == "Template")) {
        d->m_sectionList->setCurrentItem(selectItem, 0, QItemSelectionModel::ClearAndSelect);
    } else if (d->m_sectionList->selectedItems().isEmpty() && firstItem) {
        d->m_sectionList->setCurrentItem(firstItem, 0, QItemSelectionModel::ClearAndSelect);
    }
}

void KoOpenPane::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->accept();
    }
}

void KoOpenPane::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() > 0) {
        // XXX: when the MVC refactoring is done, this can open a bunch of
        //      urls, but since the part/document combination is still 1:1
        //      that won't work for now.
        Q_EMIT openExistingFile(event->mimeData()->urls().first());
    }
}

void KoOpenPane::addCustomDocumentWidget(QWidget *widget, const QString &title, const QString &icon)
{
    Q_ASSERT(widget);

    QString realtitle = title;

    if (realtitle.isEmpty())
        realtitle = i18n("Custom Document");

    QString realicon = icon;
    if (realicon.isEmpty())
        realicon = koIconName("document-new");

    QTreeWidgetItem *item = addPane(realtitle, realicon, widget, d->m_freeCustomWidgetIndex);
    ++d->m_freeCustomWidgetIndex;
    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");

    QString lastActiveItem = cfgGrp.readEntry("LastReturnType");
    bool showCustomItemByDefault = cfgGrp.readEntry("ShowCustomDocumentWidgetByDefault", false);
    if (lastActiveItem == realtitle || (lastActiveItem.isEmpty() && showCustomItemByDefault)) {
        d->m_sectionList->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
        KoSectionListItem *selectedItem = static_cast<KoSectionListItem *>(item);
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    }
}

QTreeWidgetItem *KoOpenPane::addPane(const QString &title, const QString &iconName, QWidget *widget, int sortWeight)
{
    if (!widget) {
        return nullptr;
    }

    int id = d->m_widgetStack->addWidget(widget);
    KoSectionListItem *listItem = new KoSectionListItem(d->m_sectionList, title, sortWeight, id);
    listItem->setIcon(0, QIcon::fromTheme(iconName));

    return listItem;
}

QTreeWidgetItem *KoOpenPane::addPane(const QString &title, const QPixmap &icon, QWidget *widget, int sortWeight)
{
    if (!widget) {
        return nullptr;
    }

    int id = d->m_widgetStack->addWidget(widget);
    KoSectionListItem *listItem = new KoSectionListItem(d->m_sectionList, title, sortWeight, id);

    if (!icon.isNull()) {
        QImage image = icon.toImage();

        if ((image.width() > 48) || (image.height() > 48)) {
            image = image.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        image = image.convertToFormat(QImage::Format_ARGB32);
        image = image.copy((image.width() - 48) / 2, (image.height() - 48) / 2, 48, 48);
        listItem->setIcon(0, QIcon(QPixmap::fromImage(image)));
    }

    return listItem;
}

void KoOpenPane::updateSelectedWidget()
{
    if (!d->m_sectionList->selectedItems().isEmpty()) {
        KoSectionListItem *section = dynamic_cast<KoSectionListItem *>(d->m_sectionList->selectedItems().first());

        if (!section)
            return;

        d->m_widgetStack->setCurrentIndex(section->widgetIndex());
    }
}

void KoOpenPane::saveSplitterSizes(KoDetailsPane *sender, const QList<int> &sizes)
{
    Q_UNUSED(sender);
    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
    cfgGrp.writeEntry("DetailsPaneSplitterSizes", sizes);
}

void KoOpenPane::itemClicked(QTreeWidgetItem *item)
{
    KoSectionListItem *selectedItem = static_cast<KoSectionListItem *>(item);

    if (selectedItem && selectedItem->widgetIndex() >= 0) {
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    }
}
