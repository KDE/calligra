/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QAction>

#include <kxmlguiclient.h>

#include <KoZoomHandler.h>

class KoCanvasControllerWidget;
class Canvas;
class MainWindow;
class KoZoomAction;
class KoZoomController;
class RootSection;
class Section;
class SectionsBoxDock;
class SectionPropertiesDock;
class KoCutController;
class KoCopyController;

class View : public QWidget, public KXMLGUIClient
{
    Q_OBJECT
public:

    /**
     * Constructor
     * @param document the document of this view
     * @param parent the parent widget
     */
    explicit View(RootSection* document, MainWindow* parent);
    virtual ~View();

    KoZoomHandler* zoomHandler() {
        return &m_zoomHandler;
    }

    KoZoomController *zoomController() {
        return m_zoomController;
    }

    Canvas* canvas() {
        return m_canvas;
    }
    Canvas* canvas() const {
        return m_canvas;
    }

    RootSection* rootSection() {
        return m_doc;
    }
    /// @return Page that is shown in the canvas
    Section* activeSection() const;

    /// Set page shown in the canvas to @p page
    void setActiveSection(Section * page);

private:
    /// creates the widgets (called from the constructor)
    void initGUI();
    /// creates the actions (called from the constructor)
    void initActions();
    void loadExtensions();
protected Q_SLOTS:
    void slotZoomChanged(KoZoomMode::Mode mode, qreal zoom);

    void editPaste();
    void editDeleteSelection();
    void editSelectAll();
    void editDeselectAll();

    /// Called when the mouse position changes on the canvas
    virtual void updateMousePosition(const QPoint& position);

    /// Called when the clipboard changed
    virtual void clipboardDataChanged();

private Q_SLOTS:
    void canvasReceivedFocus();
    void documentRectChanged(const QRectF& rect);
    void selectionDuplicate();
    void groupSelection();
    void ungroupSelection();
protected:
    virtual void focusInEvent(QFocusEvent * event);
private:
    void createCanvas(Section* _currentSection);
private:
    RootSection* m_doc;
    Canvas* m_canvas;
    Section* m_activeSection;
    MainWindow* m_mainWindow;


    SectionsBoxDock* m_sectionsBoxDock;
    SectionPropertiesDock* m_sectionPropertiesDock;

    KoCanvasControllerWidget * m_canvasController;
    KoZoomController * m_zoomController;
    KoZoomHandler m_zoomHandler;

    KoCutController* m_cutController;
    KoCopyController* m_copyController;
    QAction* m_editCopy;
    QAction* m_editCut;

    QAction *m_editPaste;
    QAction *m_deleteSelectionAction;
    QAction* m_groupShapes;
    QAction* m_ungroupShapes;
    KoZoomAction *m_zoomAction;
};

#endif /* KOPAVIEW_H */
