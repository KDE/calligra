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
    FileItem()
        : checked(true)
    {
    }

    QImage thumbnail;
    QString name;
    QString date;
    bool checked;
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
        QWidget *page = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(page);

        QCheckBox *checkBox = new QCheckBox;
        checkBox->setProperty("fileitem", index.data());

        connect(checkBox, &QAbstractButton::toggled, m_parent, &KoAutoSaveRecoveryDialog::toggleFileItem);
        QLabel *thumbnail = new QLabel;
        QLabel *filename = new QLabel;
        QLabel *dateModified = new QLabel;

        layout->addWidget(checkBox);
        layout->addWidget(thumbnail);
        layout->addWidget(filename);
        layout->addWidget(dateModified);

        page->setFixedSize(600, 200);

        return QList<QWidget *>() << page;
    }

    void updateItemWidgets(const QList<QWidget *> &widgets, const QStyleOptionViewItem &option, const QPersistentModelIndex &index) const override
    {
        FileItem *fileItem = (FileItem *)index.data().value<void *>();

        QWidget *page = widgets[0];
        QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(page->layout());
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(layout->itemAt(0)->widget());
        QLabel *thumbnail = qobject_cast<QLabel *>(layout->itemAt(1)->widget());
        QLabel *filename = qobject_cast<QLabel *>(layout->itemAt(2)->widget());
        QLabel *modified = qobject_cast<QLabel *>(layout->itemAt(3)->widget());

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
        itemView()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, 0);
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
    FileItemModel(QList<FileItem *> fileItems, QObject *parent)
        : QAbstractListModel(parent)
        , m_fileItems(fileItems)
    {
    }

    ~FileItemModel() override
    {
        qDeleteAll(m_fileItems);
        m_fileItems.clear();
    }

    int rowCount(const QModelIndex & /*parent*/) const override
    {
        return m_fileItems.size();
    }

    Qt::ItemFlags flags(const QModelIndex & /*index*/) const override
    {
        Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
        return flags;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (index.isValid() && index.row() < m_fileItems.size()) {
            FileItem *item = m_fileItems.at(index.row());

            switch (role) {
            case Qt::DisplayRole: {
                return QVariant::fromValue<void *>((void *)item);
            }
            case Qt::SizeHintRole:
                return QSize(600, 200);
            }
        }
        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant & /*value*/, int role) override
    {
        if (index.isValid() && index.row() < m_fileItems.size()) {
            if (role == Qt::CheckStateRole) {
                m_fileItems.at(index.row())->checked = !m_fileItems.at(index.row())->checked;
                return true;
            }
        }
        return false;
    }
    QList<FileItem *> m_fileItems;
};

KoAutoSaveRecoveryDialog::KoAutoSaveRecoveryDialog(const QStringList &filenames, QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18nc("@title:window", "Recover Files"));
    setMinimumSize(650, 500);
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    if (filenames.size() == 1) {
        layout->addWidget(new QLabel(i18n("The following autosave file can be recovered:")));
    } else {
        layout->addWidget(new QLabel(i18n("The following autosave files can be recovered:")));
    }

    m_listView = new QListView();
    m_listView->setAcceptDrops(false);
    KWidgetItemDelegate *delegate = new FileItemDelegate(m_listView, this);
    m_listView->setItemDelegate(delegate);

    QList<FileItem *> fileItems;
    for (const QString &filename : filenames) {
        FileItem *file = new FileItem();
        file->name = filename;

        QString path = QDir::homePath() + "/" + filename;
        // get thumbnail -- all calligra apps save a thumbnail
        KoStore *store = KoStore::createStore(path, KoStore::Read);

        if (store && (store->open(QString("Thumbnails/thumbnail.png")) || store->open(QString("preview.png")))) {
            // Hooray! No long delay for the user...
            QByteArray bytes = store->read(store->size());
            store->close();
            delete store;
            QImage img;
            img.loadFromData(bytes);
            file->thumbnail = img.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        // get the date
        QDateTime date = QFileInfo(path).lastModified();
        file->date = "(" + QLocale().toString(date) + ")";

        fileItems.append(file);
    }

    m_model = new FileItemModel(fileItems, m_listView);
    m_listView->setModel(m_model);
    layout->addWidget(m_listView);
    setMainWidget(page);
}

QStringList KoAutoSaveRecoveryDialog::recoverableFiles()
{
    QStringList files;
    foreach (FileItem *fileItem, m_model->m_fileItems) {
        if (fileItem->checked) {
            files << fileItem->name;
        }
    }
    return files;
}

void KoAutoSaveRecoveryDialog::toggleFileItem(bool toggle)
{
    // I've made better man from a piece of putty and matchstick!
    QVariant v = sender()->property("fileitem");
    if (v.isValid()) {
        FileItem *fileItem = (FileItem *)v.value<void *>();
        fileItem->checked = toggle;
    }
}
