 /* This file is part of the KDE project
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef HTMLCONVERT_H
#define HTMLCONVERT_H

#include <QHash>
#include <QString>
#include <QList>

#include <KoFilter.h>

class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class EpubFile;

struct StyleInfo {
    StyleInfo();

    QString parent;
    bool hasBreakBefore;
    bool inUse;
    QHash<QString, QString> attributes;
};

/** Parse all styles in the store, convert them to CSS styles and return info about them.
 *
 * This function opens contents.xml and styles.xml and parses the
 * character and paragraph properties of them. It also records the
 * parent name, wether it has the fo:break-before="page" attribute and
 * wether it is in use in the contents.
 *
 * @param odfStore the store where content.xml and styles.xml can be found.
 * @param styles the out parameter where the styles are returned. This
 * is a QHash with the style internal style name as index (not the
 * printed name) and a StyleInfo pointer as value
 *
 * @return KoFilter::OK if the parsing was successful
 * @return other if the parsing was not successful
 */
 
KoFilter::ConversionStatus convertStyles(KoStore *odfStore,
                                         QHash<QString, StyleInfo*> &styles);
KoFilter::ConversionStatus convertContent(KoStore *odfStore, QHash<QString, QString> &metaData,
                                          EpubFile *epub,QHash<QString, StyleInfo*> &styles,
                                          QHash<QString, QSizeF> &imagesSrcList);

#endif // HTMLCONVERT_H

