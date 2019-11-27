/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2003,2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * Copyright (C) 2002,2005 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002,2005,2007 David Faure <faure@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2005-2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005-2006 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2005-2006 Sven Langkamp <sven.langkamp@gmail.com>
 * Copyright (C) 2006 Martin Ellis <martin.ellis@kdemail.net>
 * Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <KoPAView.h>
#include <KoShapeAlignCommand.h>
#include <KoShapeDistributeCommand.h>
#include <karbonui_export.h>
#include "KarbonBooleanCommand.h"

class QDropEvent;
class QResizeEvent;
class QLayout;

class KarbonDocument;

class KoColor;
class KoUnit;


class KarbonPaletteBarWidget;
class KarbonPart;

class KoPACanvas;
class KoCanvasResourceManager;

class KARBONUI_EXPORT KarbonView : public KoPAView
{
    Q_OBJECT

public:
    KarbonView(KarbonPart *part, KarbonDocument* doc, QWidget* parent = 0);
    ~KarbonView() override;

    /// Returns the view is attached to
   KarbonDocument * part() const;

    /// Returns the canvas widget of this view
    KoPACanvas *canvasWidget() const;

    void reorganizeGUI();
    void setNumberOfRecentFiles(uint number);

    /// Reimplemented from QWidget
    virtual void setCursor(const QCursor &);
    /// Reimplemented from QWidget
    void dropEvent(QDropEvent *e) override;


    KoCanvasResourceManager *resourceManager() const;

    KarbonPaletteBarWidget *colorBar() const;

public Q_SLOTS:
    // editing:
    void editSelectAll();
    void editDeselectAll();
    void editDeleteSelection();

    void selectionDuplicate();
    void selectionDistributeHorizontalCenter();
    void selectionDistributeHorizontalGap();
    void selectionDistributeHorizontalLeft();
    void selectionDistributeHorizontalRight();
    void selectionDistributeVerticalCenter();
    void selectionDistributeVerticalGap();
    void selectionDistributeVerticalBottom();
    void selectionDistributeVerticalTop();

    void fileImportGraphic();

    void clipObjects();
    void unclipObjects();

    void flipVertical();
    void flipHorizontal();

    void closePath();
    void combinePath();
    void separatePath();
    void reversePath();

    void intersectPaths();
    void subtractPaths();
    void unitePaths();
    void excludePaths();

    void pathSnapToGrid();

    void selectionChanged() override;

    void editGuides();
    void showPalette();

    void replaceActivePage(KoPAPageBase *page, KoPAPageBase *newActivePage);

protected Q_SLOTS:
    // Object related operations.

    // View
    void viewModeChanged(bool);
    void zoomSelection();
    void zoomDrawing();

    void mousePositionChanged(const QPoint &position);

    void applyFillToSelection();
    void applyStrokeToSelection();
    void applyPaletteColor(const KoColor &color);

protected:
    /// Use own configuaration dialog
    void openConfiguration() override;

    void updateReadWrite(bool readwrite) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent * event) override;

    void createStrokeDock();
    void createColorDock();

private:
    void initActions();
    void updateRuler();

    void selectionDistribute(KoShapeDistributeCommand::Distribute distribute);
    void booleanOperation(KarbonBooleanCommand::BooleanOperation operation);
    void selectionFlip(bool horizontally, bool vertically);

    /// Returns a list of all selected path shapes
    QList<KoPathShape*> selectedPathShapes();

    class Private;
    Private * const d;
};

#endif // __KARBON_VIEW__

