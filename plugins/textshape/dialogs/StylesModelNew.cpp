#include "StylesModelNew.h"

#include <KoCharacterStyle.h>
#include <KoStyleThumbnailer.h>

#include <QDebug>

StylesModelNew::StylesModelNew(QObject *parent)
: QAbstractListModel(parent)
, m_styleThumbnailer(0)
{
}

QVariant StylesModelNew::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if (row < 0 || row >= m_styles.size()) {
        return QVariant();
    }
    //qDebug() << Q_FUNC_INFO << row << role;

    QVariant retval;
    switch (role) {
    case Qt::DisplayRole:
        retval = m_styles[row]->name();
        break;
    case Qt::DecorationRole:
        if (!m_styleThumbnailer) {
            retval = QPixmap();
        }
        else {
            retval = m_styleThumbnailer->thumbnail(m_styles[row]);
        }
        break;
    case StylePointer:
        retval = QVariant::fromValue(m_styles[row]);
        break;
    case Qt::SizeHintRole:
        retval = QVariant(QSize(250, 48));
    default: break;
    };
    return retval;
}

int StylesModelNew::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_styles.size();
}

void StylesModelNew::setStyleThumbnailer(KoStyleThumbnailer *thumbnailer)
{
    m_styleThumbnailer = thumbnailer;
}

void StylesModelNew::setStyles(const QList<KoCharacterStyle *> styles)
{
    beginResetModel();
    m_styles = styles;
    endResetModel();
}

void StylesModelNew::addStyle(KoCharacterStyle *style)
{
    int row = m_styles.indexOf(style);
    //Q_ASSERT(row == -1);
    if (row == -1) {
        beginInsertRows(QModelIndex(), m_styles.size(), m_styles.size());
        m_styles.append(style);
        endInsertRows();
    }
}

void StylesModelNew::removeStyle(KoCharacterStyle *style)
{
    int row = m_styles.indexOf(style);
    Q_ASSERT(row != -1);
    if (row != -1) {
        beginRemoveRows(QModelIndex(), row, row);
        m_styles.removeAt(row);
        endRemoveRows();
    }
}

void StylesModelNew::replaceStyle(KoCharacterStyle *oldStyle, KoCharacterStyle *newStyle)
{
    qDebug() << Q_FUNC_INFO << oldStyle << "->" << newStyle;
    int row = m_styles.indexOf(oldStyle);
    Q_ASSERT(row != -1);
    if (row != -1) {
        m_styles[row] = newStyle;
        QModelIndex index = this->index(row);
        emit dataChanged(index, index);
    }
}

void StylesModelNew::updateStyle(KoCharacterStyle *style)
{
    int row = m_styles.indexOf(style);
    Q_ASSERT(row != -1);
    if (row != -1) {
        qDebug() << Q_FUNC_INFO << style << style->name();
        m_styleThumbnailer->removeFromCache(style);
        QModelIndex index = this->index(row);
        emit dataChanged(index, index);
    }
}

QModelIndex StylesModelNew::styleIndex(KoCharacterStyle *style)
{
    QModelIndex index;
    int row = m_styles.indexOf(style);
    if (row != -1) {
        index = this->index(row);
    }
    return index;
}



