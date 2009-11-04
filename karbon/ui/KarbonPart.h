/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2004-2005 Laurent Montel <montel@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2004-2005,2007 David Faure <faure@kde.org>
   Copyright (C) 2004,2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>

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

#ifndef KARBON_PART_H
#define KARBON_PART_H

#include <QMap>

#include <KoShapeControllerBase.h>

#include <KoDocument.h>
#include <KoUnit.h>
#include <KarbonDocument.h>

#include <karbonui_export.h>

class QRect;
class KoDataCenter;

/**
 * Keeps track of visual per document properties.
 * It loads initial settings and applies them to the document and its views.
 */
class KARBONUI_EXPORT KarbonPart : public KoDocument, public KoShapeControllerBase
{
    Q_OBJECT
public:
    explicit KarbonPart( QWidget* parentWidget = 0L, const char* widgetName = 0L,
                QObject* parent = 0L, const char* name = 0L, bool singleViewMode = false );
    virtual ~KarbonPart();

    /// reimplemented form KoDocument
    virtual void paintContent( QPainter& painter, const QRect& rect);
    /// reimplemented form KoDocument
    virtual bool loadXML( const KoXmlDocument& document, KoStore *store );
    /// reimplemented form KoDocument
    virtual bool loadOdf( KoOdfReadStore & odfStore );
    /// reimplemented form KoDocument
    virtual bool completeLoading( KoStore* store );
    /// reimplemented form KoDocument
    virtual bool saveOdf( SavingContext &documentContext );

    /// implemented from KoShapeController
    virtual void addShape( KoShape* shape );
    /// implemented from KoShapeController
    virtual void removeShape( KoShape* shape );
    /// implemented from KoShapeController
    virtual QMap<QString, KoDataCenter*> dataCenterMap() const;

    /// Gives access to document content
    KarbonDocument& document();

    /// Returns if status bar is shown
    bool showStatusBar() const;
    /// Shows/hides status bar
    void setShowStatusBar( bool b );
    /// update attached view(s) on the current doc settings
    /// at this time only the status bar is handled
    void reorganizeGUI();

    /// Returns maximum number of recent files
    uint maxRecentFiles() const;

    /// Sets page layout of the document
    virtual void setPageLayout( const KoPageLayout& layout );

    bool mergeNativeFormat( const QString & file );

public slots:
    void slotDocumentRestored();

protected:
    /// reimplemented form KoDocument
    virtual KoView* createViewInstance( QWidget* parent );
    /// reimplemented form KoDocument
    virtual void removeView( KoView *view );

    /// Loads settings like grid and guide lines from given xml document
    void loadOasisSettings( const KoXmlDocument & settingsDoc );
    /// Saves settings like grid and guide lines to store
    void saveOasisSettings( KoStore * store );

    /// Sets given page size to all attached views/canvases
    void setPageSize( const QSizeF &pageSize );

    /// Reads settings from config file
    void initConfig();

protected slots:
    /// reimplemented from KoDocument
    virtual void openTemplate(const KUrl& url);

    /// change the unit
    void updateUnit( const KoUnit &unit );

private:
    class Private;
    Private * const d;
};

#endif // KARBON_PART_H

