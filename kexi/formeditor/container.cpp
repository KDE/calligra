/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008 Jaros�aw Staniek <staniek@kde.org>

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

#include <qpainter.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qevent.h>
#include <qlayout.h>
#include <qcursor.h>
#include <QGridLayout>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>

#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>

#include <cstdlib> // for abs()

#include "utils.h"
#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"
#include "formmanager.h"
#include "commands.h"
#include "events.h"
#include <kexiutils/utils.h>

#define KEXI_NO_FLOWLAYOUT
#ifdef __GNUC__
#warning "Port Kexi flow layout!"
#endif
#ifndef KEXI_NO_FLOWLAYOUT
#include <kexiutils/FlowLayout.h>
#endif

using namespace KFormDesigner;

EventEater::EventEater(QWidget *widget, QObject *container)
        : QObject(container)
{
    m_widget = widget;
    m_container = container;

    installRecursiveEventFilter(m_widget, this);
}

bool
EventEater::eventFilter(QObject *, QEvent *ev)
{
    if (!m_container)
        return false;

    // When the user click the empty part of tab bar, only MouseReleaseEvent is sent,
    // we need to simulate the Press event
    if (ev->type() == QEvent::MouseButtonRelease && m_widget->inherits("QTabWidget")) {
        QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
        if (mev->button() == Qt::LeftButton) {
            QMouseEvent *myev = new QMouseEvent(QEvent::MouseButtonPress, mev->pos(), mev->button(), mev->buttons(), mev->modifiers());
            m_container->eventFilter(m_widget, myev);
            delete myev;
            //return true;
        }
    }
// else if(ev->type() == QEvent::ChildInserted) {
    // widget's children have changed, we need to reinstall filter
//  installRecursiveEventFilter(m_widget, this);
// }

    return m_container->eventFilter(m_widget, ev);
}

EventEater::~EventEater()
{
    if (m_widget)
        removeRecursiveEventFilter(m_widget, this);
}

// Container itself

#ifdef __GNUC__
#warning Container::Private: move more m_* here
#else
#pragma WARNING( Container::Private: move more m_* here )
#endif
class Container::Private
{
public:
    Private(Container* toplevel, QWidget *container)
      : m_toplevel(toplevel)
      , m_widget(container)
      , insertBegin(-1, -1)
      , state(DoingNothing)
    {
    }
    ~Private() {
    }

    // for inserting/selection
    void startSelectionOrInsertingRectangle(const QPoint& begin)
    {
        insertBegin = begin;
    }
    void stopSelectionRectangleOrInserting()
    {
        insertBegin = QPoint(-1, -1);
        insertRect = QRect();
        m_widget->update();
    }
    void updateSelectionOrInsertingRectangle(const QPoint& end)
    {
        if (!selectionOrInsertingStarted()) {
            stopSelectionRectangleOrInserting();
            return;
        }
        insertRect.setTopLeft( QPoint(
            qMin(insertBegin.x(), end.x()),
            qMin(insertBegin.y(), end.y()) ) );
        insertRect.setBottomRight( QPoint(
            qMax(insertBegin.x(), end.x()),
            qMax(insertBegin.y(), end.y()) ) );
        m_widget->update();
    }
    bool selectionOrInsertingStarted() const
    {
        return insertBegin != QPoint(-1, -1);
    }
    QRect selectionOrInsertingRectangle() const {
        return insertRect;
    }
    QPoint selectionOrInsertingBegin() const {
        return insertBegin;
    }
    Container* toplevel() {
        return m_toplevel;
    }
    QWidget* widget() const
    {
        return m_widget;
    }
    void widgetDeleted() {
        m_widget = 0;
    }
    QPointer<Form> form;
    enum State {
      DoingNothing,
      DrawingSelectionRect,
      CopyingWidget,
      MovingWidget,
      InlineEditing
    };
    State state;
private:
    // the watched container and it's toplevel one...
    QPointer<Container> m_toplevel;
    QPointer<QWidget> m_widget;

    QPoint insertBegin;
    QRect insertRect;
};

Container::Container(Container *toplevel, QWidget *container, QObject *parent)
        : QObject(parent)
        , m_mousePressEventReceived(false)
        , m_mouseReleaseEvent(QEvent::None, QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier)
        , d(new Private(toplevel, container))
{
    m_moving = 0;
    m_tree = 0;
    if (toplevel)
        d->form = toplevel->form();
    m_layout = 0;
    m_layType = NoLayout;

    QByteArray classname = container->metaObject()->className();
    if ((classname == "HBox") || (classname == "Grid") || (classname == "VBox") ||
            (classname == "HFlow")  || (classname == "VFlow"))
        m_margin = 4; // those containers don't have frames, so little margin
    else
        m_margin = d->form ? d->form->defaultMargin() : 0;
    m_spacing = d->form ? d->form->defaultSpacing() : 0;

    if (toplevel) {
        ObjectTreeItem *it = new ObjectTreeItem(d->form->library()->displayName(classname),
                                                widget()->objectName(), widget(), this, this);
        setObjectTree(it);

        if (parent->isWidgetType()) {
            QString n = parent->objectName();
            ObjectTreeItem *parent = d->form->objectTree()->lookup(n);
            d->form->objectTree()->addItem(parent, it);
        } else
            d->form->objectTree()->addItem(toplevel->objectTree(), it);

        connect(toplevel, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
    }

    connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
}

Container::~Container()
{
    kDebug() << " Container being deleted this == " << objectName();
    delete d;
}

Form* Container::form() const
{
    return d->form;
}

QWidget* Container::widget() const
{
    return d->widget();
}

void
Container::setForm(Form *form)
{
    d->form = form;
    m_margin = d->form ? d->form->defaultMargin() : 0;
    m_spacing = d->form ? d->form->defaultSpacing() : 0;
}

bool
Container::eventFilter(QObject *s, QEvent *e)
{
// kDebug() << e->type();
    switch (e->type()) {
    case QEvent::MouseButtonPress: {
        d->stopSelectionRectangleOrInserting();
        m_mousePressEventReceived = true;

        kDebug() << "QEvent::MouseButtonPress sender object = " << s->objectName()
            << "of type " << s->metaObject()->className();
        kDebug() << "QEvent::MouseButtonPress this          = " << this->objectName();

        m_moving = static_cast<QWidget*>(s);
        QMouseEvent *mev = static_cast<QMouseEvent*>(e);
        m_grab = QPoint(mev->x(), mev->y());

        // we are drawing a connection
        if (FormManager::self()->isCreatingConnection())  {
            drawConnection(mev);
            return true;
        }

        if ((mev->modifiers() == Qt::ControlModifier || mev->modifiers() == Qt::ShiftModifier)
                && !FormManager::self()->isInserting()) { // multiple selection mode
            if (d->form->selectedWidgets()->contains(m_moving)) { // widget is already selected
                if (d->form->selectedWidgets()->count() > 1) // we remove it from selection
                    unSelectWidget(m_moving);
                else { // the widget is the only selected, so it means we want to copy it
                    //m_copyRect = m_moving->geometry();
                    d->state = Private::CopyingWidget;
                    if (d->form->formWidget())
                        d->form->formWidget()->initBuffer();
                }
            } else // the widget is not yet selected, we add it
                setSelectedWidget(m_moving, true, (mev->button() == Qt::RightButton));
        } else if ((d->form->selectedWidgets()->count() > 1)) { //&& (!d->form->manager()->isInserting())) // more than one widget selected
            if (!d->form->selectedWidgets()->contains(m_moving)) {
                // widget is not selected, it becomes the only selected widget
                setSelectedWidget(m_moving, false, (mev->button() == Qt::RightButton));
            }
            // If the widget is already selected, we do nothing (to ease widget moving, etc.)
        } else {// if(!d->form->manager()->isInserting())
            setSelectedWidget(m_moving, false, (mev->button() == Qt::RightButton));
        }

        // we are inserting a widget or drawing a selection rect in the form
        if ((/*s == m_container &&*/ FormManager::self()->isInserting()) || ((s == widget()) && !d->toplevel())) {
            int tmpx, tmpy;
            if (!FormManager::self()->snapWidgetsToGrid() || (mev->buttons() == Qt::LeftButton && mev->modifiers() == (Qt::ControlModifier | Qt::AltModifier))) {
                tmpx = mev->x();
                tmpy = mev->y();
            } else {
                int gridX = d->form->gridSize();
                int gridY = d->form->gridSize();
                tmpx = int((float)mev->x() / ((float)gridX) + 0.5);   // snap to grid
                tmpx *= gridX;
                tmpy = int((float)mev->y() / ((float)gridY) + 0.5);
                tmpy *= gridX;
            }

            d->startSelectionOrInsertingRectangle( (static_cast<QWidget*>(s))->mapTo(widget(), QPoint(tmpx, tmpy)) );
            if (d->form->formWidget())
                d->form->formWidget()->initBuffer();

            if (!FormManager::self()->isInserting())
                d->state = Private::DrawingSelectionRect;
        } else {
            if (s->inherits("QTabWidget")) // to allow changing page by clicking tab
                return false;
        }

        if (m_objectForMouseReleaseEvent) {
            const bool res = handleMouseReleaseEvent(m_objectForMouseReleaseEvent, &m_mouseReleaseEvent);
            m_objectForMouseReleaseEvent = 0;
            return res;
        }
        return true;
    }

    case QEvent::MouseButtonRelease: {
        QMouseEvent *mev = static_cast<QMouseEvent*>(e);
        if (!m_mousePressEventReceived) {
            m_mouseReleaseEvent = *mev;
            m_objectForMouseReleaseEvent = s;
            return true;
        }
        m_mousePressEventReceived = false;
        m_objectForMouseReleaseEvent = 0;
        return handleMouseReleaseEvent(s, mev);
    }

    case QEvent::MouseMove: {
        QMouseEvent *mev = static_cast<QMouseEvent*>(e);
        if (d->selectionOrInsertingStarted() && FormManager::self()->isInserting() && ((mev->buttons() == Qt::LeftButton) || (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier) ||
                (mev->buttons() == Qt::LeftButton && mev->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) || (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ShiftModifier)))
            // draw the insert rect
        {
//reimpl.            drawInsertRect(mev, s);
            return true;
        }
        // Creating a connection, we highlight sender and receiver, and we draw a link between them
        else if (FormManager::self()->isCreatingConnection() && !FormManager::self()->createdConnection()->sender().isNull()) {
            ObjectTreeItem *tree = d->form->objectTree()->lookup(FormManager::self()->createdConnection()->sender());
            if (!tree || !tree->widget())
                return true;

            if (d->form->formWidget() && (tree->widget() != s))
                d->form->formWidget()->highlightWidgets(tree->widget(), static_cast<QWidget*>(s));
        } else if (d->selectionOrInsertingStarted() 
                   && s == widget()
                   && !d->toplevel()
                   && (mev->modifiers() != Qt::ControlModifier)
                   && !FormManager::self()->isCreatingConnection())
        { // draw the selection rect
            if ((mev->buttons() != Qt::LeftButton) || /*m_inlineEditing*/ d->state == Private::InlineEditing)
                return true;
            d->updateSelectionOrInsertingRectangle(mev->pos());
/* moved
            int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
            int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
            int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
            int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
            QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));
            m_insertRect = r; 
*/

//! @todo?            if (d->form->formWidget())
//! @todo?                d->form->formWidget()->drawRect(r, 1); 

            d->state = Private::DoingNothing;
            return true;
        } else if (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier) {
            // draw the insert rect for the copied widget
            if (s == widget())// || (d->form->selectedWidgets()->count() > 1))
                return true;
 //moved..           drawCopiedWidgetRect(mev);
            return true;
        } else if (
            (mev->buttons() == Qt::LeftButton || (mev->buttons() == Qt::LeftButton && mev->modifiers() == (Qt::ControlModifier | Qt::AltModifier)))
            && !FormManager::self()->isInserting() && d->state != Private::CopyingWidget) {
            // we are dragging the widget(s) to move it
            if (!d->toplevel() && m_moving == widget()) // no effect for form
                return false;
            if ((!m_moving) || (!m_moving->parentWidget()))// || (m_moving->parentWidget()->inherits("QWidgetStack")))
                return true;

            moveSelectedWidgetsBy(mev->x() - m_grab.x(), mev->y() - m_grab.y());
            d->state = Private::MovingWidget;
        }

        return true; // eat
    }

    case QEvent::Paint: { // Draw the dotted background
        if (s != widget())
            return false;
//        QPaintEvent *pe = static_cast<QPaintEvent*>(e);
        int gridX = d->form->gridSize();
        int gridY = d->form->gridSize();

        QPainter p(widget());
        QColor c1(Qt::white);
        c1.setAlpha(100);
        QColor c2(Qt::black);
        c2.setAlpha(100);
        QPen pen1(c1, 1);
        QPen pen2(c2, 1);
        int cols = widget()->width() / gridX;
        int rows = widget()->height() / gridY;
        for (int rowcursor = 1; rowcursor <= rows; ++rowcursor) {
            for (int colcursor = 1; colcursor <= cols; ++colcursor) {
                const int x = -1 + colcursor *gridX;
                const int y = -1 + rowcursor *gridY;
                p.setPen(pen1);
                p.drawPoint(x, y);
                p.drawPoint(x, y+1);
                p.setPen(pen2);
                p.drawPoint(x+1, y);
                p.drawPoint(x+1, y+1);
            }
        }

        if (d->selectionOrInsertingRectangle().isValid()) {
            QColor sc1(Qt::white);
            sc1.setAlpha(220);
            QColor sc2(Qt::black);
            sc2.setAlpha(200);
            QPen selPen1(sc2, 1.0);
            QPen selPen2(sc1, 1.0, Qt::CustomDashLine);
            QVector<qreal> dashes;
            dashes << 2 << 2;
            selPen2.setDashPattern(dashes);
            p.setPen(selPen1);
            p.drawRect(d->selectionOrInsertingRectangle());
            p.setPen(selPen2);
            p.drawRect(d->selectionOrInsertingRectangle());
        }
        return false;
    }

    case QEvent::Resize: { // we are resizing a widget, so we set m_move to true -> the layout will be reloaded when releasing mouse
        if (d->form->interactiveMode())
            d->state = Private::MovingWidget;
        break;
    }

    //case QEvent::AccelOverride:
    case QEvent::KeyPress: {
        QKeyEvent *kev = static_cast<QKeyEvent*>(e);

        if (kev->key() == Qt::Key_F2) { // pressing F2 == double-clicking
            d->state = Private::InlineEditing;
            QWidget *w;

            // try to find the widget which was clicked last and should be edited
            if (d->form->selectedWidgets()->count() == 1)
                w = d->form->selectedWidgets()->first();
            else if (d->form->selectedWidgets()->contains(m_moving))
                w = m_moving;
            else
                w = d->form->selectedWidgets()->last();
            d->form->library()->startEditing(w->metaObject()->className(), w, this);
        } else if (kev->key() == Qt::Key_Escape) {
            if (FormManager::self()->isCreatingConnection())
                FormManager::self()->stopCreatingConnection();
            else if (FormManager::self()->isInserting())
                FormManager::self()->stopInsert();
            return true;
        } else if ((kev->key() == Qt::Key_Control) && (d->state == Private::MovingWidget)) {
            if (!m_moving)
                return true;
            // we simulate a mouse move event to update screen
            QMouseEvent *mev = new QMouseEvent(QEvent::MouseMove, m_moving->mapFromGlobal(QCursor::pos()), Qt::NoButton,
                                               Qt::LeftButton, Qt::ControlModifier);
            eventFilter(m_moving, mev);
            delete mev;
        } else if (kev->key() == FormManager::self()->contextMenuKey()) {
            FormManager::self()->createContextMenu(static_cast<QWidget*>(s), this, false);
            return true;
        } else if (kev->key() == Qt::Key_Delete) {
            FormManager::self()->deleteWidget();
            return true;
        }
        // directional buttons move the widget
        else if (kev->key() == Qt::Key_Left) { // move the widget of gridX to the left
            moveSelectedWidgetsBy(-form()->gridSize(), 0);
            return true;
        } else if (kev->key() == Qt::Key_Right) { // move the widget of gridX to the right
            moveSelectedWidgetsBy(form()->gridSize(), 0);
            return true;
        } else if (kev->key() == Qt::Key_Up) { // move the widget of gridY to the top
            moveSelectedWidgetsBy(0, - form()->gridSize());
            return true;
        } else if (kev->key() == Qt::Key_Down) { // move the widget of gridX to the bottom
            moveSelectedWidgetsBy(0, form()->gridSize());
            return true;
        } else if ((kev->key() == Qt::Key_Tab) || (kev->key() == Qt::Key_Backtab)) {
            ObjectTreeItem *item = form()->objectTree()->lookup(
                form()->selectedWidgets()->first()->objectName()
            );
            if (!item || !item->parent())
                return true;
            ObjectTreeList *list = item->parent()->children();
            if (list->count() == 1)
                return true;
            int index = list->indexOf(item);

            if (kev->key() == Qt::Key_Backtab) {
                if (index == 0) // go back to the last item
                    index = list->count() - 1;
                else
                    index = index - 1;
            } else  {
                if (index == int(list->count() - 1)) // go back to the first item
                    index = 0;
                else
                    index = index + 1;
            }

            ObjectTreeItem *nextItem = list->at(index);
            if (nextItem && nextItem->widget())
                form()->setSelectedWidget(nextItem->widget(), false);
        }
        return true;
    }

    case QEvent::KeyRelease: {
        QKeyEvent *kev = static_cast<QKeyEvent*>(e);
        if ((kev->key() == Qt::Key_Control) && (d->state == Private::CopyingWidget)) {
            // cancel copying
            //m_copyRect = QRect();
            if (d->form->formWidget())
                d->form->formWidget()->clearForm();
        }
        return true;
    }

    case QEvent::MouseButtonDblClick: { // editing
        kDebug() << "Container: Mouse dbl click for widget " << s->objectName();
        QWidget *w = static_cast<QWidget*>(s);
        if (!w)
            return false;

        //m_inlineEditing = true;
        d->state = Private::InlineEditing;
        d->form->library()->startEditing(w->metaObject()->className(), w, this);
        return true;
    }

    case QEvent::ContextMenu:
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::FocusIn:
    case QEvent::FocusOut:
//  case QEvent::DragEnter:
//  case QEvent::DragMove:
//  case QEvent::DragLeave:
        return true; // eat them

    default:
        return false; // let the widget do the rest ...
    }
    return false;
}

bool
Container::handleMouseReleaseEvent(QObject *s, QMouseEvent *mev)
{
    if (FormManager::self()->isInserting()) { // we insert the widget at cursor pos
        if (d->form->formWidget())
            d->form->formWidget()->clearForm();
        K3Command *com = new InsertWidgetCommand(this/*, mev->pos()*/);
        d->form->addCommand(com, true);
        d->stopSelectionRectangleOrInserting();
        return true;
    } else if (s == widget() && !d->toplevel() && (mev->button() != Qt::RightButton) && d->selectionOrInsertingRectangle().isValid()) {
        // we are still drawing a rect to select widgets
        d->stopSelectionRectangleOrInserting();
//reimpl.        drawSelectionRect(mev);
        return true;
    }
    if (mev->button() == Qt::RightButton) { // Right-click -> context menu
        FormManager::self()->createContextMenu(static_cast<QWidget*>(s), this);
    } else if (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier) { // && (m_copyRect.isValid()))
        // copying a widget by Ctrl+dragging

        if (d->form->formWidget())
            d->form->formWidget()->clearForm();
        if (s == widget()) // should have no effect on form
            return true;

        // prevent accidental copying of widget (when moving mouse a little while selecting)
        if (((mev->pos().x() - m_grab.x()) < form()->gridSize() && (m_grab.x() - mev->pos().x()) < form()->gridSize()) &&
                ((mev->pos().y() - m_grab.y()) < form()->gridSize() && (m_grab.y() - mev->pos().y()) < form()->gridSize())) {
            kDebug() << "The widget has not been moved. No copying";
            return true;
        }

        d->form->setInteractiveMode(false);
        // We simulate copy and paste
        FormManager::self()->copyWidget();
        if (d->form->selectedWidgets()->count() > 1)
            FormManager::self()->setInsertPoint(mev->pos());
        else
            FormManager::self()->setInsertPoint(static_cast<QWidget*>(s)->mapTo(widget(), mev->pos() - m_grab));
        FormManager::self()->pasteWidget();
        d->form->setInteractiveMode(true);

        //m_initialPos = QPoint();
    } else if (d->state == Private::MovingWidget) {
        // one widget has been moved, so we need to update the layout
        reloadLayout();
    }

    // cancel copying as user released Ctrl before releasing mouse button
    d->stopSelectionRectangleOrInserting();
    d->state = Private::DoingNothing;
    return true; // eat
}

void
Container::setSelectedWidget(QWidget *w, bool add, bool dontRaise, bool moreWillBeSelected)
{
    if (w)
        kDebug() << "slotSelectionChanged " << w->objectName();

    if (!w) {
        d->form->setSelectedWidget(widget());
        return;
    }

    d->form->setSelectedWidget(w, add, dontRaise, moreWillBeSelected);
}

void
Container::unSelectWidget(QWidget *w)
{
    if (!w)
        return;

    d->form->unSelectWidget(w);
}

Container*
Container::toplevel()
{
    if (d->toplevel())
        return d->toplevel();
    else
        return this;
}

void
Container::deleteWidget(QWidget *w)
{
    if (!w)
        return;
// kDebug() << "Deleting a widget: " << w->objectName();
    d->form->objectTree()->removeItem(w->objectName());
    FormManager::self()->deleteWidgetLater(w);
    d->form->setSelectedWidget(widget());
}

void
Container::widgetDeleted()
{
    d->widgetDeleted();
    deleteLater();
}

/// Layout functions

void
Container::setLayout(LayoutType type)
{
    if (m_layType == type)
        return;

    delete m_layout;
    m_layout = 0;
    m_layType = type;

    switch (type) {
    case HBox: {

        m_layout = static_cast<QLayout*>( new QHBoxLayout(widget()) );
        m_layout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
        m_layout->setSpacing(m_spacing);
        createBoxLayout(new HorizontalWidgetList(d->form->toplevelContainer()->widget()));

        break;
    }
    case VBox: {

        m_layout = static_cast<QLayout*>( new QVBoxLayout(widget()) );
        m_layout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
        m_layout->setSpacing(m_spacing);
        createBoxLayout(new VerticalWidgetList(d->form->toplevelContainer()->widget()));

        break;
    }
    case Grid: {
        createGridLayout();
        break;
    }
    case  HFlow: {
#ifndef KEXI_NO_FLOWLAYOUT

        KexiFlowLayout *flow = new KexiFlowLayout(widget());
        flow->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
        flow->setSpacing(m_spacing);

        flow->setOrientation(Qt::Horizontal);
        m_layout = (QLayout*)flow;
        createFlowLayout();
#endif
        break;
    }
    case VFlow: {
#ifndef KEXI_NO_FLOWLAYOUT
        KexiFlowLayout *flow = new KexiFlowLayout(widget(), m_margin, m_spacing);
        flow->setOrientation(Qt::Vertical);
        m_layout = (QLayout*)flow;
        createFlowLayout();
#endif
        break;
    }
    default: {
        m_layType = NoLayout;
        return;
    }
    }
    widget()->setGeometry(widget()->geometry()); // just update layout
    m_layout->activate();
}

void
Container::reloadLayout()
{
    LayoutType type = m_layType;
    setLayout(NoLayout);
    setLayout(type);
}

void
Container::createBoxLayout(CustomSortableWidgetList* list)
{
    QBoxLayout *layout = static_cast<QBoxLayout*>(m_layout);

    foreach (ObjectTreeItem *titem, *m_tree->children()) {
        list->append(titem->widget());
    }
    list->sort();

    foreach (QWidget *w, *list) {
        layout->addWidget(w);
    }
    delete list;
}

void
Container::createFlowLayout()
{
#ifndef KEXI_NO_FLOWLAYOUT
    KexiFlowLayout *flow = dynamic_cast<KexiFlowLayout*>(m_layout);
    if (!flow || m_tree->children()->isEmpty())
        return;

    const int offset = 15;
    QWidgetList *list = 0, *list2 = 0;
    if (flow->orientation() == Qt::Horizontal) {
        list = new VerticalWidgetList(d->form->toplevelContainer()->widget());
        list2 = new HorizontalWidgetList(d->form->toplevelContainer()->widget());
    } else {
        list = new HorizontalWidgetList(d->form->toplevelContainer()->widget());
        list2 = new VerticalWidgetList(d->form->toplevelContainer()->widget());
    }

    // fill the list
    foreach (ObjectTreeItem *titem, *m_tree->children()) {
        list->append(titem->widget());
    }
    list->sort();

    if (flow->orientation() == Qt::Horizontal) {
        int y = list->first()->y();
        foreach (QWidget *w, *list) {
            if ((w->y() > y + offset)) {
                // start a new line
                list2->sort();
                foreach (QWidget *w2, *list2) {
                    flow->add(w2);
                }
                list2->clear();
                y = w->y();
            }
            list2->append(w);
        }

        list2->sort(); // don't forget the last line
        foreach (QWidget *w, *list2) {
            flow->add(w);
        }
    } else {
        int x = list->first()->x();
        foreach (QWidget *w, *list) {
            if ((w->x() > x + offset)) {
                // start a new column
                list2->sort();
                foreach (QWidget *w2, *list2) {
                    flow->add(w2);
                }
                list2->clear();
                x = w->x();
            }
            list2->append(w);
        }

        list2->sort(); // don't forget the last column
        foreach (QWidget *w, *list2) {
            flow->add(w);
        }
    }

    delete list;
    delete list2;
#endif
}

void
Container::createGridLayout(bool testOnly)
{
    //Those lists sort widgets by y and x
    VerticalWidgetList *vlist = new VerticalWidgetList(d->form->toplevelContainer()->widget());
    HorizontalWidgetList *hlist = new HorizontalWidgetList(d->form->toplevelContainer()->widget());
    // The vector are used to store the x (or y) beginning of each column (or row)
    QVector<int> cols;
    QVector<int> rows;
    int end = -1000;
    bool same = false;

    foreach (ObjectTreeItem *titem, *m_tree->children()) {
        vlist->append(titem->widget());
    }
    vlist->sort();

    foreach (ObjectTreeItem *titem, *m_tree->children()) {
        hlist->append(titem->widget());
    }
    hlist->sort();

    // First we need to make sure that two widgets won't be in the same row,
    // ie that no widget overlap another one
    if (!testOnly) {
        for (QWidgetList::ConstIterator it(vlist->constBegin()); it!=vlist->constEnd(); ++it) {
            QWidget *w = *it;
            for (QWidgetList::ConstIterator it2(it); it2!=vlist->constEnd(); ++it2) {
                QWidget *nextw = *it2;
                if ((w->y() >= nextw->y()) || (nextw->y() >= w->geometry().bottom()))
                    break;

                if (!w->geometry().intersects(nextw->geometry()))
                    break;
                // If the geometries of the two widgets intersect each other,
                // we move one of the widget to the rght or bottom of the other
                if ((nextw->y() - w->y()) > abs(nextw->x() - w->x()))
                    nextw->move(nextw->x(), w->geometry().bottom() + 1);
                else if (nextw->x() >= w->x())
                    nextw->move(w->geometry().right() + 1, nextw->y());
                else
                    w->move(nextw->geometry().right() + 1, nextw->y());
            }
        }
    }

    // Then we count the number of rows in the layout, and set their beginnings
    for (QWidgetList::ConstIterator it(vlist->constBegin()); it!=vlist->constEnd(); ++it) {
        QWidget *w = *it;
        if (!same) { // this widget will make a new row
            end = w->geometry().bottom();
            rows.append(w->y());
        }
        QWidgetList::ConstIterator it2(it);

        // If same == true, it means we are in the same row as prev widget
        // (so no need to create a new column)
        ++it2;
        if (it2==vlist->constEnd())
            break;

        QWidget *nextw = *it2;
        if (nextw->y() >= end)
            same = false;
        else {
            same = !(same && (nextw->y() >= w->geometry().bottom()));
            if (!same)
                end = w->geometry().bottom();
        }
    }
    kDebug() << "the new grid will have n rows: n == " << rows.size();

    end = -10000;
    same = false;
    // We do the same thing for the columns
    for (QWidgetList::ConstIterator it(hlist->constBegin()); it!=hlist->constEnd(); ++it) {
        QWidget *w = *it;
        if (!same) {
            end = w->geometry().right();
            cols.append(w->x());
        }

        QWidgetList::ConstIterator it2(it);
        ++it2;
        if (it2==hlist->constEnd())
            break;

        QWidget *nextw = *it2;
        if (nextw->x() >= end)
            same = false;
        else {
            same = !(same && (nextw->x() >= w->geometry().right()));
            if (!same)
                end = w->geometry().right();
        }
    }
    kDebug() << "the new grid will have n columns: n == " << cols.size();

    // We create the layout ..
    QGridLayout *layout = 0;
    if (!testOnly) {

        layout = new QGridLayout(widget()); //, rows.size(), cols.size(), m_margin, m_spacing);

        layout->setObjectName("grid");
//! @todo allow for individual margins and spacing
        layout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
        layout->setSpacing(m_spacing);
        m_layout = static_cast<QLayout*>(layout);
    }

    // .. and we fill it with widgets
    for (QWidgetList::ConstIterator it(vlist->constBegin()); it!=vlist->constEnd(); ++it) {
        QWidget *w = *it;
        QRect r( w->geometry() );
        uint wcol = 0, wrow = 0, endrow = 0, endcol = 0;
        uint i = 0;

        // We look for widget row(s) ..
        while (r.y() >= rows[i]) {
            if ((uint)rows.size() <= (i + 1)) { // we are the last row
                wrow = i;
                break;
            }
            if (r.y() < rows[i+1]) {
                wrow = i; // the widget will be in this row
                uint j = i + 1;
                // Then we check if the widget needs to span multiple rows
                while ((uint)rows.size() >= (j + 1) && r.bottom() > rows[j]) {
                    endrow = j;
                    j++;
                }

                break;
            }
            i++;
        }
        //kDebug() << "the widget " << w->objectName() << " wil be in the row " << wrow <<
        //" and will go to the row " << endrow;

        // .. and column(s)
        i = 0;
        while (r.x() >= cols[i]) {
            if ((uint)cols.size() <= (i + 1)) { // last column
                wcol = i;
                break;
            }
            if (r.x() < cols[i+1]) {
                wcol = i;
                uint j = i + 1;
                // Then we check if the widget needs to span multiple columns
                while ((uint)cols.size() >= (j + 1) && r.right() > cols[j]) {
                    endcol = j;
                    j++;
                }

                break;
            }
            i++;
        }
        //kDebug() << "the widget " << w->objectName() << " wil be in the col " << wcol <<
        // " and will go to the col " << endcol;

        ObjectTreeItem *item = d->form->objectTree()->lookup(w->objectName());
        if (!endrow && !endcol) {
            if (!testOnly)
                layout->addWidget(w, wrow, wcol);
            item->setGridPos(wrow, wcol, 0, 0);
        } else {
            if (!endcol)
                endcol = wcol;
            if (!endrow)
                endrow = wrow;
            if (!testOnly)
                layout->addWidget(w, wrow, wcol, endrow - wrow + 1, endcol - wcol + 1);
            item->setGridPos(wrow, wcol, endrow - wrow + 1, endcol - wcol + 1);
        }
    }
}

QString
Container::layoutTypeToString(int type)
{
    switch (type) {
    case HBox: return "HBox";
    case VBox: return "VBox";
    case Grid: return "Grid";
    case HFlow: return "HFlow";
    case VFlow: return "VFlow";
    default:   return "NoLayout";
    }
}

Container::LayoutType
Container::stringToLayoutType(const QString &name)
{
    if (name == "HBox") return HBox;
    if (name == "VBox") return VBox;
    if (name == "Grid") return Grid;
    if (name == "HFlow")  return HFlow;
    if (name == "VFlow")  return VFlow;
    return NoLayout;
}

/// Drawing functions used by eventFilter
void
Container::drawConnection(QMouseEvent *mev)
{
    if (mev->button() != Qt::LeftButton) {
        FormManager::self()->resetCreatedConnection();
        return;
    }
    // First click, we select the sender and display menu to choose signal
    if (FormManager::self()->createdConnection()->sender().isNull()) {
        FormManager::self()->createdConnection()->setSender(m_moving->objectName());
        if (d->form->formWidget()) {
            d->form->formWidget()->initBuffer();
            d->form->formWidget()->highlightWidgets(m_moving, 0/*, QPoint()*/);
        }
        FormManager::self()->createSignalMenu(m_moving);
        return;
    }
    // the user clicked outside the menu, we cancel the connection
    if (FormManager::self()->createdConnection()->signal().isNull()) {
        FormManager::self()->stopCreatingConnection();
        return;
    }
    // second click to choose the receiver
    if (FormManager::self()->createdConnection()->receiver().isNull()) {
        FormManager::self()->createdConnection()->setReceiver(m_moving->objectName());
        FormManager::self()->createSlotMenu(m_moving);
        widget()->repaint();
        return;
    }
    // the user clicked outside the menu, we cancel the connection
    if (FormManager::self()->createdConnection()->slot().isNull()) {
        FormManager::self()->stopCreatingConnection();
        return;
    }
}

#if 0 // reimplemented using QRubberBand 
void
Container::drawSelectionRect(QMouseEvent *mev)
{
    //finish drawing unclipped selection rectangle: clear the surface
    if (d->form->formWidget())
        d->form->formWidget()->clearForm();
    d->updateSelectionOrInsertingRectangle(mev->pos());
/*moved
    int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
    int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
    int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
    int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
    QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));*/

    setSelectedWidget(widget(), false);
    QWidget *widgetToSelect = 0;
    // We check which widgets are in the rect and select them
    foreach (ObjectTreeItem *titem, *m_tree->children()) {
        QWidget *w = titem->widget();
        if (!w)
            continue;
        if (w->geometry().intersects(r) && w != widget()) {
            if (widgetToSelect)
                setSelectedWidget(widgetToSelect, true/*add*/, false/*raise*/, true/*moreWillBeSelected*/);
            widgetToSelect = w; //select later
        }
    }
    if (widgetToSelect) //the last one left
        setSelectedWidget(widgetToSelect, true/*add*/, false/*raise*/, false/*!moreWillBeSelected*/);

    m_insertRect = QRect();
    d->state = Private::DoingNothing;
    widget()->repaint();
}

void
Container::drawInsertRect(QMouseEvent *mev, QObject *s)
{
    int tmpx, tmpy;
    QPoint pos = static_cast<QWidget*>(s)->mapTo(widget(), mev->pos());
    int gridX = d->form->gridSize();
    int gridY = d->form->gridSize();
    if (!FormManager::self()->snapWidgetsToGrid()
        || (mev->buttons() == Qt::LeftButton && mev->modifiers() == (Qt::ControlModifier | Qt::AltModifier))) {
        tmpx = pos.x();
        tmpy = pos.y();
    } else {
        tmpx = int((float) pos.x() / ((float)gridX) + 0.5);
        tmpx *= gridX;
        tmpy = int((float)pos.y() / ((float)gridY) + 0.5);
        tmpy *= gridX;
    }

    int topx = (m_insertBegin.x() < tmpx) ? m_insertBegin.x() : tmpx;
    int topy = (m_insertBegin.y() < tmpy) ? m_insertBegin.y() : tmpy;
    int botx = (m_insertBegin.x() > tmpx) ? m_insertBegin.x() : tmpx;
    int boty = (m_insertBegin.y() > tmpy) ? m_insertBegin.y() : tmpy;
    m_insertRect = QRect(QPoint(topx, topy), QPoint(botx, boty));

    if (m_insertRect.x() < 0)
        m_insertRect.setLeft(0);
    if (m_insertRect.y() < 0)
        m_insertRect.setTop(0);
    if (m_insertRect.right() > m_container->width())
        m_insertRect.setRight(m_container->width());
    if (m_insertRect.bottom() > m_container->height())
        m_insertRect.setBottom(m_container->height());

    if (FormManager::self()->isInserting() && m_insertRect.isValid()) {
        if (d->form->formWidget()) {
            QRect drawRect = QRect(m_container->mapTo(d->form->widget(), m_insertRect.topLeft())
                                   , m_insertRect.size());
            d->form->formWidget()->drawRect(drawRect, 2);
        }
    }
}

void
Container::drawCopiedWidgetRect(QMouseEvent *mev)
{
    // We've been dragging a widget, but Ctrl was hold, so we start copy
    if (d->state == Private::MovingWidget)  {
        //FormManager::self()->undo(); // undo last moving
        //m_moving->move(m_initialPos);
        if (d->form->formWidget())  {
            m_container->repaint();
            d->form->formWidget()->initBuffer();
        }
        d->state = Private::CopyingWidget;
    }

    //m_copyRect.moveTopLeft(m_container->mapFromGlobal( mev->globalPos()) - m_grab);

    if (d->form->formWidget())  {
        QList<QRect> rectList;
        foreach (QWidget *w, *d->form->selectedWidgets()) {
            QRect drawRect = w->geometry();
            QPoint p = mev->pos() - m_grab;
            drawRect.moveTo(drawRect.x() + p.x(), drawRect.y() + p.y());
            p = m_container->mapTo(d->form->widget(), QPoint(0, 0));
            //drawRect = QRect( ((QWidget*)s)->mapTo(d->form->widget(), drawRect.topLeft()), drawRect.size());
            drawRect.moveTo(drawRect.x() + p.x(), drawRect.y() + p.y());
            rectList.append(drawRect);
        }

        d->form->formWidget()->drawRects(rectList, 2);
    }
}
#endif

/// Other functions used by eventFilter
void
Container::moveSelectedWidgetsBy(int realdx, int realdy, QMouseEvent *mev)
{
    if (d->form->selectedWidget() == d->form->widget())
        return; //do not move top-level widget

    const int gridX = d->form->gridSize();
    const int gridY = d->form->gridSize();
    int dx = realdx, dy = realdy;

    foreach (QWidget *w, *d->form->selectedWidgets()) {
        if (!w->parent() || w->parent()->inherits("QTabWidget") || w->parent()->inherits("QWidgetStack"))
            continue;

        if (w->parentWidget() && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")) {
            w = w->parentWidget(); // widget is WidgetStack page
            if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
                w = w->parentWidget();
        }

        int tmpx = w->x() + realdx;
        int tmpy = w->y() + realdy;
        if (tmpx < 0)
            dx = qMax(0 - w->x(), dx); // because dx is <0
        else if (tmpx > w->parentWidget()->width() - gridX)
            dx = qMin(w->parentWidget()->width() - gridX - w->x(), dx);

        if (tmpy < 0)
            dy = qMax(0 - w->y(), dy); // because dy is <0
        else if (tmpy > w->parentWidget()->height() - gridY)
            dy = qMin(w->parentWidget()->height() - gridY - w->y(), dy);
    }

    foreach (QWidget *w, *d->form->selectedWidgets()) {
        // Don't move tab widget pages (or widget stack pages)
        if (!w->parent() || w->parent()->inherits("QTabWidget") || w->parent()->inherits("QWidgetStack"))
            continue;

        if (w->parentWidget() && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")) {
            w = w->parentWidget(); // widget is WidgetStack page
            if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
                w = w->parentWidget();
        }

        int tmpx, tmpy;
        if (!FormManager::self()->snapWidgetsToGrid() || (mev && mev->buttons() == Qt::LeftButton && mev->modifiers() == (Qt::ControlModifier | Qt::AltModifier))) {
            tmpx = w->x() + dx;
            tmpy = w->y() + dy;
        } else {
            tmpx = int(float(w->x() + dx) / float(gridX) + 0.5) * gridX;
            tmpy = int(float(w->y() + dy) / float(gridY) + 0.5) * gridY;
        }

        if ((tmpx != w->x()) || (tmpy != w->y()))
            w->move(tmpx, tmpy);
    }
}

void Container::stopInlineEditing()
{
    d->state = Private::DoingNothing;
}

QRect Container::selectionOrInsertingRectangle() const {
    return d->selectionOrInsertingRectangle();
}

QPoint Container::selectionOrInsertingBegin() const {
    return d->selectionOrInsertingBegin();
}

////////////

DesignTimeDynamicChildWidgetHandler::DesignTimeDynamicChildWidgetHandler()
        : m_item(0)
{
}

DesignTimeDynamicChildWidgetHandler::~DesignTimeDynamicChildWidgetHandler()
{
}

void
DesignTimeDynamicChildWidgetHandler::childWidgetAdded(QWidget* w)
{
    if (m_item) {
        installRecursiveEventFilter(w, m_item->eventEater());
    }
}

#include "container.moc"
