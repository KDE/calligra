/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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

#ifndef KSPREAD_DOCBASE_H
#define KSPREAD_DOCBASE_H

#include <KoDocument.h>

#include "kspread_export.h"

class KoOasisSettings;
class KoResourceManager;

namespace KSpread
{
class Map;
class SheetAccessModel;

class KSPREAD_EXPORT DocBase : public KoDocument
{
    Q_OBJECT
public:
    /**
     * \ingroup OpenDocument
     */
    enum SaveFlag { SaveAll, SaveSelected }; // kpresenter and kword have have SavePage too

    /**
     * Creates a new document.
     * @param parentWidget the parent widget
     * @param parent the parent object
     * @param singleViewMode enables single view mode, if @c true
     */
    explicit DocBase(QWidget* parentWidget = 0, QObject* parent = 0, bool singleViewMode = false);
    ~DocBase();

    /**
     * @return list of all documents
     */
    static QList<DocBase*> documents();

    virtual void setReadWrite(bool readwrite = true);

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
    KoResourceManager *resourceManager() const;

    SheetAccessModel *sheetAccessModel() const;

    virtual void initConfig();



    /**
     * \ingroup OpenDocument
     * Main saving method.
     */
    virtual bool saveOdf(SavingContext &documentContext);

    /**
     * \ingroup OpenDocument
     * Save the whole document, or just the selection, into OASIS format
     * When saving the selection, also return the data as plain text and/or plain picture,
     * which are used to insert into the KMultipleDrag drag object.
     *
     * @param store the KoStore to save into
     * @param manifestWriter pointer to a koxmlwriter to add entries to the manifest
     * @param saveFlag either the whole document, or only the selected text/objects.
     * @param plainText must be set when saveFlag==SaveSelected.
     *        It returns the plain text format of the saved data, when available.
     */
    bool saveOdfHelper(SavingContext &documentContext, SaveFlag saveFlag,
                       QString* plainText = 0);

    /**
     * \ingroup OpenDocument
     * Main loading method.
     * @see Map::loadOdf
     */
    virtual bool loadOdf(KoOdfReadStore & odfStore);

protected:
    class Private;
    Private * const d;

    virtual void paintContent(QPainter & painter, const QRect & rect);
    virtual bool loadXML(const KoXmlDocument& doc, KoStore *store);
    virtual KoView* createViewInstance(QWidget* parent);
private:
    Q_DISABLE_COPY(DocBase)

    /**
     * \ingroup OpenDocument
     * Saves the Document related settings.
     * The actual saving takes place in Map::saveOdfSettings.
     * @see Map::saveOdfSettings
     */
    void saveOdfSettings(KoXmlWriter &settingsWriter);

    /**
     * \ingroup OpenDocument
     * Loads the Document related settings.
     * The actual loading takes place in Map::loadOdfSettings.
     * @see Map::loadOdfSettings
     */
    void loadOdfSettings(const KoXmlDocument&settingsDoc);

    /**
     * \ingroup OpenDocument
     * Load the spell checker ignore list.
     */
    void loadOdfIgnoreList(const KoOasisSettings& settings);
};

} // namespace KSpread

#endif // KSPREAD_DOCBASE_H
