#ifndef FAKE_KWIDGETITEMDELEGATE_H
#define FAKE_KWIDGETITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QAbstractItemView>

class KWidgetItemDelegate : public QStyledItemDelegate
{
public:
    KWidgetItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent), m_itemView(0) {}
    KWidgetItemDelegate(QAbstractItemView *itemView, QObject *parent = 0) : QStyledItemDelegate(parent), m_itemView(itemView) {}
    QAbstractItemView *itemView() const { return m_itemView; }
private:
    QAbstractItemView *m_itemView;
};

#endif
