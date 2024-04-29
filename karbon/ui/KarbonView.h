/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2001-2005, 2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002-2003, 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002, 2005 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2002, 2005, 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005-2006 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2005-2006 Sven Langkamp <sven.langkamp@gmail.com>
 * SPDX-FileCopyrightText: 2006 Martin Ellis <martin.ellis@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include "KarbonBooleanCommand.h"
#include <KoPAView.h>
#include <KoShapeAlignCommand.h>
#include <KoShapeDistributeCommand.h>
#include <karbonui_export.h>

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
    KarbonView(KarbonPart *part, KarbonDocument *doc, QWidget *parent = nullptr);
    ~KarbonView() override;

    /// Returns the view is attached to
    KarbonDocument *part() const;

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
    /// Use own configuration dialog
    void openConfiguration() override;

    void updateReadWrite(bool readwrite) override;
    void resizeEvent(QResizeEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

    void createStrokeDock();
    void createColorDock();

private:
    void initActions();
    void updateRuler();

    void selectionDistribute(KoShapeDistributeCommand::Distribute distribute);
    void booleanOperation(KarbonBooleanCommand::BooleanOperation operation);
    void selectionFlip(bool horizontally, bool vertically);

    /// Returns a list of all selected path shapes
    QList<KoPathShape *> selectedPathShapes();

    class Private;
    Private *const d;
};

#endif // __KARBON_VIEW__
