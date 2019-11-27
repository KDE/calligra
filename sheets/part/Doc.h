/* This file is part of the KDE project
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

#ifndef CALLIGRA_SHEETS_DOC
#define CALLIGRA_SHEETS_DOC

#include <QList>
#include <QMap>
#include <QRect>
#include <QString>

#include <KoDocument.h>
#include <KoXmlReader.h>

#include "../Global.h"
#include "../DocBase.h"

#include "sheets_common_export.h"

class QDomDocument;
class QPainter;

class KoStore;
class KoXmlWriter;
class KoPart;

namespace Calligra
{
namespace Sheets
{
class Sheet;
class Doc;
class View;
class Map;

/**
 * This class holds the data that makes up a spreadsheet.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT Doc : public DocBase
{
    Q_OBJECT
    Q_PROPERTY(int syntaxVersion READ syntaxVersion)

public:
    /**
     * Creates a new document.
     * @param part The KoPart that owns the document. XXX: should be removed!
     */
    explicit Doc(KoPart *part);

    /**
     * Destroys the document.
     */
    ~Doc() override;



    bool completeSaving(KoStore* _store) override;


    /**
     * \ingroup NativeFormat
     * Main saving method.
     */
    QDomDocument saveXML() override;

    /**
     * \ingroup NativeFormat
     * Main loading method.
     */
    bool loadXML(const KoXmlDocument& doc, KoStore *store) override;

    int supportedSpecialFormats() const override;

    virtual bool loadChildren(KoStore* _store);

    bool docData(QString const & xmlTag, QDomDocument & data);

    // reimplemented; paints the thumbnail
    void paintContent(QPainter & painter, const QRect & rect) override;
    virtual void paintContent(QPainter & painter, const QRect & rect, Sheet* sheet);

    void initConfig() override;
    void saveConfig();

    void addIgnoreWordAll(const QString & word);
    void clearIgnoreWordAll();
    void addIgnoreWordAllList(const QStringList & _lst);
    QStringList spellListIgnoreAll() const ;

    /* Function specific when we load config from file */
    void loadConfigFromFile();
    bool configLoadFromFile() const;

    bool saveOdf(SavingContext &documentContext) override;

    /**
     * Requests an update of all attached user interfaces (views).
     */
    void updateAllViews();


public Q_SLOTS:
    void initEmpty() override;

Q_SIGNALS:
    /**
     * Emitted, if all user interfaces (views) have to be updated.
     */
    void updateView();

    /**
     * Emitted, if all editors have to be closed.
     */
    void closeEditor(bool);

protected Q_SLOTS:
    void sheetAdded(Sheet* sheet);

protected:

    /**
     * @reimp Overloaded function of KoDocument.
     */
    bool completeLoading(KoStore*) override;

private:
    Q_DISABLE_COPY(Doc)

    class Private;
    Private * const dd;

    /**
     * \ingroup NativeFormat
     */
    void loadPaper(KoXmlElement const & paper);
};

} // namespace Sheets
} // namespace Calligra

#endif /* CALLIGRA_SHEETS_DOC */
