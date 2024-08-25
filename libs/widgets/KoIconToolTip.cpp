/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 Carsten Pfeiffer <pfeiffer@kde.org>
   SPDX-FileCopyrightText: 2002 Igor Jansen <rm@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoIconToolTip.h"

#include <QBuffer>
#include <QImage>
#include <QTextDocument>
#include <QUrl>

#include <KoResourceModel.h>

QString KoIconToolTip::createDocument(const QModelIndex &index) const
{
    QTextDocument doc;

    const QString name = index.data(Qt::DisplayRole).toString();
    const auto thumb = index.data(KoResourceModel::LargeThumbnailRole).value<QImage>();

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    thumb.save(&buffer, "png");
    QString base64 = QString::fromUtf8(byteArray.toBase64());

    const QString image = QStringLiteral("<image src=\"data:image/png;base64,%1\">").arg(base64);
    const QString body = QStringLiteral("<h3 align=\"center\">%1</h3>").arg(name) + image;
    const QString html = QStringLiteral("<html><body>%1</body></html>").arg(body);

    doc.setHtml(html);
    doc.setTextWidth(qMin(doc.size().width(), qreal(500.0)));

    return doc.toHtml();
}
