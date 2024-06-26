/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DOCBASE_H
#define CALLIGRA_SHEETS_DOCBASE_H

#include <KoDocument.h>

#include "sheets_core_export.h"

class KoDocumentResourceManager;
class KoPart;

#define SHEETS_MIME_TYPE "application/vnd.oasis.opendocument.spreadsheet"

namespace Calligra
{
namespace Sheets
{
class Map;
class Sheet;

class CALLIGRA_SHEETS_CORE_EXPORT DocBase : public KoDocument
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
    static QList<DocBase *> documents();

    void setReadWrite(bool readwrite = true) override;

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override
    {
        return SHEETS_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override
    {
        return SHEETS_MIME_TYPE;
    }
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
     * Return a pointer to the resource manager associated with the
     * document. The resource manager contains
     * document wide resources * such as variable managers, the image
     * collection and others.
     * @see KoCanvasBase::resourceManager()
     */
    KoDocumentResourceManager *resourceManager() const;

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
    bool loadOdf(KoOdfReadStore &odfStore) override;
    bool loadXML(const KoXmlDocument &doc, KoStore *store) override;
    QDomDocument saveXML() override;

protected:
    class Private;
    Private *const d;

    void paintContent(QPainter &painter, const QRect &rect) override;

private:
    Q_DISABLE_COPY(DocBase)
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DOCBASE_H
