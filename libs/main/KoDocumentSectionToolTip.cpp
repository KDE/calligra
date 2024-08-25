/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoDocumentSectionToolTip.h"
#include "KoDocumentSectionModel.h"

#include <KLocalizedString>
#include <QBuffer>
#include <QImage>
#include <QModelIndex>
#include <QTextDocument>
#include <QUrl>

KoDocumentSectionToolTip::KoDocumentSectionToolTip() = default;

KoDocumentSectionToolTip::~KoDocumentSectionToolTip() = default;

QString KoDocumentSectionToolTip::createDocument(const QModelIndex &index) const
{
    QTextDocument doc;

    auto thumb = index.data(int(Model::BeginThumbnailRole) + 250).value<QImage>();

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    thumb.save(&buffer, "png");
    QString base64 = QString::fromUtf8(byteArray.toBase64());

    QString name = index.data(Qt::DisplayRole).toString();
    auto properties = index.data(Model::PropertiesRole).value<Model::PropertyList>();
    QString rows;
    const QString row = QStringLiteral("<tr><td align=\"right\">%1:</td><td align=\"left\">%2</td></tr>");
    QString value;
    for (qsizetype i = 0, n = properties.count(); i < n; ++i) {
        if (properties[i].isMutable) {
            value = properties[i].state.toBool() ? i18n("Yes") : i18n("No");
        } else {
            value = properties[i].state.toString();
        }

        rows.append(row.arg(properties[i].name, value));
    }

    rows = QStringLiteral("<table>%1</table>").arg(rows);

    const QString image = QStringLiteral("<table border=\"1\"><tr><td><img src=\"data:image/png;base64,%1\"></td></tr></table>").arg(base64);
    const QString body = QString("<h3 align=\"center\">%1</h3>").arg(name) + QString("<table><tr><td>%1</td><td>%2</td></tr></table>").arg(image, rows);
    const QString html = QString("<html><body>%1</body></html>").arg(body);

    doc.setHtml(html);
    doc.setTextWidth(qMin(doc.size().width(), qreal(500.0)));

    return doc.toHtml();
}
