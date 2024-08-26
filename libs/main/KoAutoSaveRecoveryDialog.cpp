/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoAutoSaveRecoveryDialog.h"

#include <KoStore.h>

#include <KLocalizedString>
#include <kwidgetitemdelegate.h>

#include <QAbstractTableModel>
#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QImage>
#include <QLabel>
#include <QListView>
#include <QPainter>
#include <QPixmap>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

struct FileItem {
    QImage thumbnail;
    QString name;
    QString date;
    bool checked = true;
};

class FileItemDelegate : public KWidgetItemDelegate
{
public:
    FileItemDelegate(QAbstractItemView *itemView, KoAutoSaveRecoveryDialog *dlg)
        : KWidgetItemDelegate(itemView, dlg)
        , m_parent(dlg)
    {
    }

    QList<QWidget *> createItemWidgets(const QModelIndex &index) const override
    {
        auto page = new QWidget;
        auto layout = new QHBoxLayout(page);

        auto checkBox = new QCheckBox;
        checkBox->setProperty("fileitem", index.data(Qt::UserRole));
        const auto fileItem = index.data(Qt::UserRole).value<FileItem *>();

        connect(checkBox, &QAbstractButton::toggled, this, [fileItem, this](bool toggle) {
            fileItem->checked = toggle;
        });
        auto thumbnail = new QLabel;
        auto filename = new QLabel;
        auto dateModified = new QLabel;

        layout->addWidget(checkBox);
        layout->addWidget(thumbnail);
        layout->addWidget(filename);
        layout->addWidget(dateModified);

        page->setFixedSize(600, 200);

        return {page};
    }

    void updateItemWidgets(const QList<QWidget *> &widgets, const QStyleOptionViewItem &option, const QPersistentModelIndex &index) const override
    {
        auto fileItem = index.data(Qt::UserRole).value<FileItem *>();

        QWidget *page = widgets[0];
        auto layout = qobject_cast<QHBoxLayout *>(page->layout());
        auto checkBox = qobject_cast<QCheckBox *>(layout->itemAt(0)->widget());
        auto thumbnail = qobject_cast<QLabel *>(layout->itemAt(1)->widget());
        auto filename = qobject_cast<QLabel *>(layout->itemAt(2)->widget());
        auto modified = qobject_cast<QLabel *>(layout->itemAt(3)->widget());

        checkBox->setChecked(fileItem->checked);
        thumbnail->setPixmap(QPixmap::fromImage(fileItem->thumbnail));
        filename->setText(fileItem->name);
        modified->setText(fileItem->date);

        // move the page _up_ otherwise it will draw relative to the actual position
        page->setGeometry(option.rect.translated(0, -option.rect.y()));
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex & /*index*/) const override
    {
        // paint background for selected or hovered item
        QStyleOptionViewItem opt = option;
        itemView()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, nullptr);
    }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override
    {
        return QSize(600, 200);
    }

    KoAutoSaveRecoveryDialog *m_parent;
};

class KoAutoSaveRecoveryDialog::FileItemModel : public QAbstractListModel
{
public:
    FileItemModel(std::vector<std::unique_ptr<FileItem>> &&fileItems, QObject *parent)
        : QAbstractListModel(parent)
        , m_fileItems(std::move(fileItems))
    {
    }

    int rowCount(const QModelIndex & /*parent*/) const override
    {
        return (int)m_fileItems.size();
    }

    Qt::ItemFlags flags(const QModelIndex & /*index*/) const override
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

        auto &item = m_fileItems.at(index.row());

        switch (role) {
        case Qt::UserRole:
            return QVariant::fromValue<FileItem *>(item.get());
        case Qt::SizeHintRole:
            return QSize(600, 200);
        default:
            return {};
        }
    }

    bool setData(const QModelIndex &index, const QVariant & /*value*/, int role) override
    {
        Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

        if (role == Qt::CheckStateRole) {
            m_fileItems.at(index.row())->checked = !m_fileItems.at(index.row())->checked;
            return true;
        }
        return false;
    }

    std::vector<std::unique_ptr<FileItem>> m_fileItems;
};

KoAutoSaveRecoveryDialog::KoAutoSaveRecoveryDialog(const QStringList &filenames, QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18nc("@title:window", "Recover Files"));
    setButtons(KoDialog::Ok | KoDialog::Cancel | KoDialog::User1);
    setButtonText(KoDialog::User1, i18n("Discard All"));
    setMinimumSize(650, 500);
    auto page = new QWidget(this);
    auto layout = new QVBoxLayout(page);
    if (filenames.size() == 1) {
        layout->addWidget(new QLabel(i18n("The following autosave file can be recovered:")));
    } else {
        layout->addWidget(new QLabel(i18n("The following autosave files can be recovered:")));
    }

    m_listView = new QListView();
    m_listView->setAcceptDrops(false);
    KWidgetItemDelegate *delegate = new FileItemDelegate(m_listView, this);
    m_listView->setItemDelegate(delegate);

    std::vector<std::unique_ptr<FileItem>> fileItems;
    for (const QString &filename : filenames) {
        auto file = std::make_unique<FileItem>();
        file->name = filename;

        QString path = QDir::homePath() + "/" + filename;
        // get thumbnail -- all calligra apps save a thumbnail
        auto store = std::unique_ptr<KoStore>(KoStore::createStore(path, KoStore::Read));

        if (store && (store->open(QString("Thumbnails/thumbnail.png")) || store->open(QString("preview.png")))) {
            // Hooray! No long delay for the user...
            QByteArray bytes = store->read(store->size());
            store->close();
            QImage img;
            img.loadFromData(bytes);
            file->thumbnail = img.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        // get the date
        QDateTime date = QFileInfo(path).lastModified();
        file->date = "(" + QLocale().toString(date) + ")";

        fileItems.push_back(std::move(file));
    }

    m_model = new FileItemModel(std::move(fileItems), m_listView);
    m_listView->setModel(m_model);

    layout->addWidget(m_listView);
    layout->addWidget(new QLabel(
        i18n("If you select Cancel, all recoverable files will be kept.\nIf you press OK, selected files will be recovered, the unselected files discarded.")));

    setMainWidget(page);
    connect(this, &KoDialog::user1Clicked, this, &KoAutoSaveRecoveryDialog::slotDeleteAll);
}

void KoAutoSaveRecoveryDialog::slotDeleteAll()
{
    for (auto &fileItem : std::as_const(m_model->m_fileItems)) {
        fileItem->checked = false;
    }
    accept();
}

QStringList KoAutoSaveRecoveryDialog::recoverableFiles()
{
    QStringList files;
    for (auto &fileItem : std::as_const(m_model->m_fileItems)) {
        if (fileItem->checked) {
            files << fileItem->name;
        }
    }
    return files;
}
