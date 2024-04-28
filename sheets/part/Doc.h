/* This file is part of the KDE project
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

#ifndef CALLIGRA_SHEETS_DOC
#define CALLIGRA_SHEETS_DOC

#include "../core/DocBase.h"

#include "sheets_part_export.h"

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
class SheetBase;
class Doc;
class View;
class Map;

/**
 * This class holds the data that makes up a spreadsheet.
 */
class CALLIGRA_SHEETS_PART_EXPORT Doc : public DocBase
{
    Q_OBJECT
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

    bool completeSaving(KoStore *_store) override;

    QDomDocument saveXML() override;

    int supportedSpecialFormats() const override;

    // reimplemented; paints the thumbnail
    void paintContent(QPainter &painter, const QRect &rect) override;
    virtual void paintContent(QPainter &painter, const QRect &rect, Sheet *sheet);

    void initConfig() override;
    void saveConfig();

    void addIgnoreWordAll(const QString &word);
    void clearIgnoreWordAll();
    void addIgnoreWordAllList(const QStringList &_lst);

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
    void sheetAdded(SheetBase *sheet);

protected:
    /**
     * @reimp Overloaded function of KoDocument.
     */
    bool completeLoading(KoStore *) override;

private:
    Q_DISABLE_COPY(Doc)

    class Private;
    Private *const dd;

    /**
     * \ingroup NativeFormat
     */
    void loadPaper(KoXmlElement const &paper);
};

} // namespace Sheets
} // namespace Calligra

#endif /* CALLIGRA_SHEETS_DOC */
