/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_SECTION_DELEGATE_H
#define KO_DOCUMENT_SECTION_DELEGATE_H

#include <QAbstractItemDelegate>
class KoDocumentSectionView;

class KoDocumentSectionModel;

/**
 * The KoDocumentSectionDelegate is the gui pendant of a
 * KoDocumentSectionModel: the graphical representation of one item in
 * a KoDocumentSectionView.
 */
class KoDocumentSectionDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit KoDocumentSectionDelegate(KoDocumentSectionView *view, QObject *parent = nullptr);
    ~KoDocumentSectionDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    using Model = KoDocumentSectionModel;
    using View = KoDocumentSectionView;

    class Private;
    std::unique_ptr<Private> const d;

    static QStyleOptionViewItem getOptions(const QStyleOptionViewItem &option, const QModelIndex &index);
    int thumbnailHeight(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    int availableWidth() const;
    int textBoxHeight(const QStyleOptionViewItem &option) const;
    QRect textRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QRect iconsRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QRect thumbnailRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QRect decorationRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QRect progressBarRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawText(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawIcons(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawThumbnail(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawDecoration(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawProgressBar(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
