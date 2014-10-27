/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_FLIPBOOK_H
#define KIS_FLIPBOOK_H

#include <QStandardItemModel>
#include <krita_export.h>

#include <KoDocument.h>

class KisFlipbookItem;
class QString;

class KRITAUI_EXPORT KisFlipbook : public KoDocument
{
    Q_OBJECT
public:
    explicit KisFlipbook(const KoPart *part= 0);
    virtual ~KisFlipbook();

    void setName(const QString &name);
    QString name() const;

    KisFlipbookItem *addItem(const QString &url);

    void loadFlipbook(const QString &url);
    void saveFlipbook(const QString &url);

    QStandardItemModel *model() const;

public: // KoDocument

    virtual QByteArray nativeFormatMimeType() const { return "application/x-krita-flipbook"; }
    virtual QByteArray nativeOasisMimeType() const { return ""; }
    virtual QStringList extraNativeMimeTypes() const { return QStringList(); }
    virtual void paintContent(QPainter &/*painter*/, const QRect &/*rect*/) {}
    virtual bool loadXML(const KoXmlDocument & /*doc*/, KoStore */*store*/) { return false; }
    virtual bool loadOdf(KoOdfReadStore &/*odfStore*/) { return false; }
    virtual bool saveOdf(SavingContext &/*documentContext*/) { return false; }

private:

    QString m_name;
    QStandardItemModel *m_model;

};

#endif // KIS_FLIPBOOK_H
