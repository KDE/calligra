/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
   Copyright (C) 2002-2003,2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002,2005 Laurent Montel <montel@kde.org>
   Copyright (C) 2002,2005,2007 David Faure <faure@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2005-2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2005-2006 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2006 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>

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

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <KoView.h>
#include <QPointF>
#include <kxmlguibuilder.h>
#include <KoUnit.h>
#include <KoShapeAlignCommand.h>
#include <KoShapeDistributeCommand.h>
#include <KoZoomMode.h>
#include <karbonui_export.h>
#include "KarbonBooleanCommand.h"

class QLabel;
class QDropEvent;
class QResizeEvent;
class QRectF;
class QPrinter;

class KAction;
class KarbonPart;
class KSelectAction;
class KToggleAction;

class KoCanvasController;
class KoRuler;

class KarbonLayerDocker;
class KarbonZoomController;

class KarbonCanvas;
class KarbonStylePreviewDocker;

class KARBONUI_EXPORT KarbonView : public KoView
{
    Q_OBJECT

public:
    explicit KarbonView(KarbonPart* part, QWidget* parent = 0);
    virtual ~KarbonView();

    /// Returns the view is attached to
    KarbonPart * part() const;

    /// Returns the canvas widget of this view
    KarbonCanvas * canvasWidget() const;

    void reorganizeGUI();
    void setNumberOfRecentFiles(uint number);

    /// Reimplemented from QWidget
    virtual void setCursor(const QCursor &);
    /// Reimplemented from QWidget
    virtual void dropEvent(QDropEvent *e);
    /// Reimplemented from KoView
    virtual KoZoomController *zoomController() const;


public slots:
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

    void closePath();
    void combinePath();
    void separatePath();
    void reversePath();

    void intersectPaths();
    void subtractPaths();
    void unitePaths();
    void excludePaths();

    void pathSnapToGrid();

    void configure();

    void pageLayout();

    void selectionChanged();

    void togglePageMargins(bool);
    void showRuler();
    void showGuides();
    void snapToGrid();

protected slots:
    // Object related operations.

    // View
    void viewModeChanged(bool);
    void zoomSelection();
    void zoomDrawing();

    void mousePositionChanged(const QPoint &position);
    void pageOffsetChanged();

    void updateUnit(KoUnit unit);

    void applyFillToSelection();
    void applyStrokeToSelection();

protected:
    virtual void resizeEvent(QResizeEvent* event);

    void createLayersTabDock();
    void createStrokeDock();
    void createColorDock();

    virtual KoPrintJob * createPrintJob();

private:
    void initActions();
    void updateRuler();

    void selectionDistribute(KoShapeDistributeCommand::Distribute distribute);

    void booleanOperation(KarbonBooleanCommand::BooleanOperation operation);

    /// Returns a list of all selected path shapes
    QList<KoPathShape*> selectedPathShapes();

    class Private;
    Private * const d;
};

#endif // __KARBON_VIEW__

