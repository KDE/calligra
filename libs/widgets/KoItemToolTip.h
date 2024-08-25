/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_ITEM_TOOLTIP_H
#define KO_ITEM_TOOLTIP_H

#include "kowidgets_export.h"

#include <QTextDocument>

class QStyleOptionViewItem;
class QModelIndex;
class QHelpEvent;
class QAbstractItemView;

/**
 * Base class for tooltips that can show extensive information about
 * the contents of the data pointed to by something that contains a
 * QModelIndex. Subclasses need to use this data to create a
 * QTextDocument that is formatted to provide the complete tooltip.
 *
 * (KoItemToolTip is currently used in kopainter/KoResourceChooser)
 */
class KOWIDGETS_EXPORT KoItemToolTip
{
public:
    KoItemToolTip();
    virtual ~KoItemToolTip();

    void showTip(QWidget *widget, const QPoint &pos, const QRect &rect, const QStyleOptionViewItem &option, const QModelIndex &index);
    void hide() const;

protected:
    /**
     * Re-implement this to provide the actual tooltip contents.
     * For instance:
     * @code
     *     QTextDocument doc;
     *
     *     QString name = index.data(Qt::DisplayRole).toString();
     *
     *     const QString image = QString("<img src=\"data:thumbnail\">");
     *     const QString body = QString("<h3 align=\"center\">%1</h3>").arg(name) + image;
     *     const QString html = QString("<html><body>%1</body></html>").arg(body);
     *
     *     doc.setHtml(html);
     *     doc.setTextWidth(qMin(doc->size().width(), 500.0));
     *
     *     return doc.toHtml();
     * @endcode
     */
    [[nodiscard]] virtual QString createDocument(const QModelIndex &index) const = 0;

private:
    class Private;
    std::unique_ptr<Private> const d;
};

#endif
