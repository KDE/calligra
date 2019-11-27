/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2005 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_DOCBASE_H
#define CALLIGRA_SHEETS_DOCBASE_H

#include <KoDocument.h>

#include "sheets_odf_export.h"

class KoDocumentResourceManager;
class KoPart;

#define SHEETS_MIME_TYPE "application/vnd.oasis.opendocument.spreadsheet"

namespace Calligra
{
namespace Sheets
{
class Map;
class Sheet;
class SheetAccessModel;

class CALLIGRA_SHEETS_ODF_EXPORT DocBase : public KoDocument
{
    Q_OBJECT
public:

    /**
     * Creates a new document.
     * @param part The KoPart that owns the document. XXX: should be removed!
     */
    explicit DocBase(KoPart *part);
    ~DocBase() override;

    /**
     * @return list of all documents
     */
    static QList<DocBase*> documents();

    void setReadWrite(bool readwrite = true) override;

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override { return SHEETS_MIME_TYPE; }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override {return SHEETS_MIME_TYPE;}
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override
    {
        return QStringList() << "application/vnd.oasis.opendocument.spreadsheet-template"
                             << "application/x-kspread";
    }

    /**
     * @return the Map that belongs to this Document
     */
    Map *map() const;

    /**
     * Returns the syntax version of the currently opened file
     */
    int syntaxVersion() const;

    /**
     * Return a pointer to the resource manager associated with the
     * document. The resource manager contains
     * document wide resources * such as variable managers, the image
     * collection and others.
     * @see KoCanvasBase::resourceManager()
     */
    KoDocumentResourceManager *resourceManager() const;

    SheetAccessModel *sheetAccessModel() const;

    virtual void initConfig();

    QStringList spellListIgnoreAll() const;
    void setSpellListIgnoreAll(const QStringList &list);


    /**
     * \ingroup OpenDocument
     * Main saving method.
     */
    bool saveOdf(SavingContext &documentContext) override;

    /**
     * \ingroup OpenDocument
     * Main loading method. Wrapper around Odf::loadDocument.
     * @see Map::loadOdf
     */
    bool loadOdf(KoOdfReadStore & odfStore) override;
protected:
    class Private;
    Private * const d;

    void paintContent(QPainter & painter, const QRect & rect) override;
    bool loadXML(const KoXmlDocument& doc, KoStore *store) override;

private:
    Q_DISABLE_COPY(DocBase)
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DOCBASE_H
