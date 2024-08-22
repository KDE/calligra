/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoDocumentSectionToolTip.h"
#include "KoDocumentSectionModel.h"

#include <KLocalizedString>
#include <QImage>
#include <QModelIndex>
#include <QTextDocument>
#include <QUrl>

KoDocumentSectionToolTip::KoDocumentSectionToolTip() = default;

KoDocumentSectionToolTip::~KoDocumentSectionToolTip() = default;

QTextDocument *KoDocumentSectionToolTip::createDocument(const QModelIndex &index)
{
    QTextDocument *doc = new QTextDocument(this);

    QImage thumb = index.data(int(Model::BeginThumbnailRole) + 250).value<QImage>();
    doc->addResource(QTextDocument::ImageResource, QUrl("data:thumbnail"), thumb);

    QString name = index.data(Qt::DisplayRole).toString();
    Model::PropertyList properties = index.data(Model::PropertiesRole).value<Model::PropertyList>();
    QString rows;
    const QString row = QString("<tr><td align=\"right\">%1:</td><td align=\"left\">%2</td></tr>");
    QString value;
    for (int i = 0, n = properties.count(); i < n; ++i) {
        if (properties[i].isMutable)
            value = properties[i].state.toBool() ? i18n("Yes") : i18n("No");
        else
            value = properties[i].state.toString();

        rows.append(row.arg(properties[i].name, value));
    }

    rows = QString("<table>%1</table>").arg(rows);

    const QString image = QString("<table border=\"1\"><tr><td><img src=\"data:thumbnail\"></td></tr></table>");
    const QString body = QString("<h3 align=\"center\">%1</h3>").arg(name) + QString("<table><tr><td>%1</td><td>%2</td></tr></table>").arg(image, rows);
    const QString html = QString("<html><body>%1</body></html>").arg(body);

    doc->setHtml(html);
    doc->setTextWidth(qMin(doc->size().width(), qreal(500.0)));

    return doc;
}
