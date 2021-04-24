/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 Carsten Pfeiffer <pfeiffer@kde.org>
   SPDX-FileCopyrightText: 2002 Igor Jansen <rm@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoIconToolTip.h"

#include <QTextDocument>
#include <QUrl>

#include <KoResourceModel.h>

// #include <WidgetsDebug.h>

QTextDocument *KoIconToolTip::createDocument( const QModelIndex &index )
{
    QTextDocument *doc = new QTextDocument( this );

    QImage thumb = index.data( KoResourceModel::LargeThumbnailRole ).value<QImage>();
    doc->addResource( QTextDocument::ImageResource, QUrl( "data:thumbnail" ), thumb );

    QString name = index.data( Qt::DisplayRole ).toString();

    const QString image = QString( "<image src=\"data:thumbnail\">" );
    const QString body = QString( "<h3 align=\"center\">%1</h3>" ).arg( name ) + image;
    const QString html = QString( "<html><body>%1</body></html>" ).arg( body );

    doc->setHtml( html );
    doc->setTextWidth( qMin( doc->size().width(), qreal(500.0) ) );

    return doc;
}
