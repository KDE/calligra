/* This file is part of the KDE project
   Copyright (C) 2011-2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiContextMessage.h"

#include <QFormLayout>
#include <QTimer>
#include <QAction>
#include <QPointer>
#include <QMouseEvent>
#include <QApplication>

#include <kexiutils/utils.h>
#include "KexiAssistantPage.h"
#include "KexiLinkWidget.h"

#include <kdebug.h>

class KexiContextMessage::Private
{
public:
    Private() : defaultAction(0), contentsWidget(0) {}
    ~Private() {
//        qDeleteAll(actions);
    }
    QString text;
    QList<QAction*> actions;
    QSet<QAction*> leftButtonAlignment;
    QAction* defaultAction;
    QWidget *contentsWidget;
};

// ----

KexiContextMessage::KexiContextMessage(const QString& text)
 : d(new Private)
{
    setText(text);
}

KexiContextMessage::KexiContextMessage(const KexiContextMessage& other)
 : d(new Private(*other.d))
{
}

KexiContextMessage::KexiContextMessage(QWidget *contentsWidget)
 : d(new Private)
{
    d->contentsWidget = contentsWidget;
}

KexiContextMessage::~KexiContextMessage()
{
    delete d;
}

QString KexiContextMessage::text() const
{
    return d->text;
}

void KexiContextMessage::setText(const QString text)
{
    d->text = text;
}

void KexiContextMessage::addAction(QAction* action, ButtonAlignment alignment)
{
    d->actions.append(action);
    if (alignment == AlignLeft) {
        d->leftButtonAlignment.insert(action);
    }
}
    
QList<QAction*> KexiContextMessage::actions() const
{
    return d->actions;
}

KexiContextMessage::ButtonAlignment KexiContextMessage::buttonAlignment(QAction* action) const
{
    return d->leftButtonAlignment.contains(action) ? AlignLeft : AlignRight;
}

void KexiContextMessage::setDefaultAction(QAction* action)
{
    d->defaultAction = action;
}

QAction* KexiContextMessage::defaultAction() const
{
    return d->defaultAction;
}

QWidget* KexiContextMessage::contentsWidget() const
{
    return d->contentsWidget;
}

// ----

class KexiContextMessageWidget::Private
{
public:
    Private()
     : resizeTrackingPolicy(0)
     , hasActions(false)
     , eventBlocking(true)
    {
    }
    ~Private() {}

    void setDisabledColorsForPage()
    {
        origPagePalette = page->palette();
        QPalette pal(page->palette());
        for (int i = 0; i < QPalette::NColorRoles; i++) {
            pal.setBrush(QPalette::Active, static_cast<QPalette::ColorRole>(i),
                        pal.brush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i)));
            pal.setBrush(QPalette::Inactive, static_cast<QPalette::ColorRole>(i),
                        pal.brush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i)));
        }
        page->setPalette(pal);
    }

    void setEnabledColorsForPage()
    {
        if (page && (hasActions || contentsWidget))
            page->setPalette(origPagePalette);
    }

    QPointer<QWidget> page;
    QList< QPointer<QWidget> > enabledLinks;
    QPalette origPagePalette;
    QPointer<QWidget> context;
    QPointer<QWidget> nextFocusWidget;
    QPointer<QWidget> contentsWidget;

    //!< For updating the callout position
    QPointer<QWidget> trackedWidget;
    QPoint origCalloutPointerPosition;
    QSize origSize;
    QSize origPageSize;
    QPoint trackedWidgetOriginalPos;
    Qt::Orientations resizeTrackingPolicy;

    bool hasActions;
    bool eventBlocking;
};

KexiContextMessageWidget::KexiContextMessageWidget(
    QWidget *page, QFormLayout* layout,
    QWidget *context, const KexiContextMessage& message)
 : KMessageWidget(message.contentsWidget(), 0)
 , d(new Private)
{
    init(page, layout, context, message);
}

KexiContextMessageWidget::KexiContextMessageWidget(
   QFormLayout* layout,
   QWidget *context, const KexiContextMessage& message)
 : KMessageWidget()
 , d(new Private)
{
    init(0, layout, context, message);
}

KexiContextMessageWidget::KexiContextMessageWidget(
    QFormLayout* layout, QWidget *context, const QString& message)
 : KMessageWidget()
 , d(new Private)
{
    KexiContextMessage contextMessage(message);
    init(0, layout, context, contextMessage);
}

void KexiContextMessageWidget::init(
    QWidget *page, QFormLayout* layout,
    QWidget *context, const KexiContextMessage& message)
{
    d->context = context;
    d->page = page;
    d->contentsWidget = message.contentsWidget();
    hide();
    setText(message.text());
    setMessageType(KMessageWidget::Warning);
    setWordWrap(true);
    setCloseButtonVisible(false);
    setAutoDelete(true);
    setContentsMargins(3, 0, 3, 0); // to better fit to line edits
    d->hasActions = !message.actions().isEmpty();
    if ((d->page && d->hasActions) || d->contentsWidget) {
        d->setDisabledColorsForPage();
        foreach (KexiLinkWidget* w, d->page->findChildren<KexiLinkWidget*>()) {
            //kDebug() << w << w->isEnabled();
            if (w->isEnabled()) {
                d->enabledLinks.append(w);
                w->setEnabled(false);
            }
        }
        KexiUtils::installRecursiveEventFilter(d->page, this); // before inserting,
                                                               // so 'this' is not disabled
    }

    if (layout) {
        int row;
        layout->getWidgetPosition(context, &row, 0);
        layout->insertRow(row, QString(), this);
        setCalloutPointerDirection(KMessageWidget::Down);
    }
    else {
        if (d->page) {
            setParent(page);
        }
    }

    if (d->hasActions) {
        foreach(QAction* action, message.actions()) {
            KMessageWidget::addAction(action);
            if (KexiContextMessage::AlignLeft == message.buttonAlignment(action)) {
                KMessageWidget::setButtonLeftAlignedForAction(action);
            }
            connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
        }

        if (message.defaultAction()) {
            setDefaultAction(message.defaultAction());
        }
    }
    else {
        if (d->context)
            d->context->setFocus();
    }
    QTimer::singleShot(10, this, SLOT(animatedShow()));
}

KexiContextMessageWidget::~KexiContextMessageWidget()
{
    d->eventBlocking = false;
    d->setEnabledColorsForPage();
    foreach (QPointer<QWidget> w, d->enabledLinks) {
        if (w) {
            w->setEnabled(true);
            w->unsetCursor();
        }
    }
    if (d->nextFocusWidget)
        d->nextFocusWidget->setFocus();
    else if (d->context)
        d->context->setFocus();
    delete d;
}

void KexiContextMessageWidget::actionTriggered()
{
    if (d->page) {
        d->page->setEnabled(true);
    }
    animatedHide();
}

bool KexiContextMessageWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (d->contentsWidget && event->type() == QEvent::MouseButtonRelease) {
        // hide the message when clicking outside when contents widget is present
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        QWidget *w = QApplication::widgetAt(me->globalPos());
        //kDebug() << watched << w << w->parentWidget();
        if (!KexiUtils::hasParent(this, w)) {
            actionTriggered();
            return true;
        }
    }
    
    if (watched == d->page && event->type() == QEvent::Resize) {
        //kDebug() << "RESIZE:" << watched;
        if (d->trackedWidget) {
            if (d->resizeTrackingPolicy != 0) {
                // update size
                //kDebug() << d->origSize << d->page->size() << d->origPageSize;
                if (!d->origSize.isValid()) {
                    d->origSize = size();
                }
                QSize newSize(d->origSize + d->page->size() - d->origPageSize);
                QSize sizeToSet = size();
                if (d->resizeTrackingPolicy & Qt::Horizontal) {
                    sizeToSet.setWidth(newSize.width());
                }
                if (d->resizeTrackingPolicy & Qt::Vertical) {
                    sizeToSet.setHeight(newSize.height());
                    setFixedHeight(newSize.height());
                }
                resize(sizeToSet);
                setPaletteInherited();
            }
            // update position
            QPoint delta(d->trackedWidget->mapToGlobal(QPoint(0, 0)) - d->trackedWidgetOriginalPos);
            KMessageWidget::setCalloutPointerPosition(d->origCalloutPointerPosition + delta);
        }
    }

    switch (event->type()) {
    case QEvent::ActivationChange:
    case QEvent::CloseSoftwareInputPanel:
    case QEvent::ContextMenu:
    case QEvent::CursorChange:
    case QEvent::DragEnter:
    case QEvent::DragLeave:
    case QEvent::DragMove:
    case QEvent::Drop:
    case QEvent::EnabledChange:
    case QEvent::Enter:
#ifdef QT_KEYPAD_NAVIGATION 
    case QEvent::EnterEditFocus:
    case QEvent::LeaveEditFocus:
#endif
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    case QEvent::IconDrag:
    case QEvent::InputMethod:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Leave:
    case QEvent::LeaveWhatsThisMode:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::NonClientAreaMouseButtonDblClick:
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseMove:
    case QEvent::QueryWhatsThis:
    case QEvent::RequestSoftwareInputPanel:
    case QEvent::Shortcut:
    case QEvent::ShortcutOverride:
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::ToolTip:
    case QEvent::WhatsThis:
    case QEvent::WhatsThisClicked:
    case QEvent::Wheel:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::TouchBegin:
#ifndef QT_NO_GESTURES
    case QEvent::Gesture:
#endif
        if (d->eventBlocking)
            return true;
    default:;
    }
    return KMessageWidget::eventFilter(watched, event);
}

void KexiContextMessageWidget::setNextFocusWidget(QWidget *widget)
{
    d->nextFocusWidget = widget;
}

void KexiContextMessageWidget::setCalloutPointerPosition(const QPoint& globalPos,
                                                         QWidget *trackedWidget)
{
    KMessageWidget::setCalloutPointerPosition(globalPos);
    // save current position so delta can be easier computed in the future
    // (see KexiContextMessageWidget::eventFilter())
    d->trackedWidget = trackedWidget;
    if (d->trackedWidget) {
        d->origCalloutPointerPosition = globalPos;
        d->origSize = QSize(-1, -1);
        d->origPageSize = d->page->size();
        d->trackedWidgetOriginalPos = d->trackedWidget->mapToGlobal(QPoint(0, 0));
    }
}

void KexiContextMessageWidget::setResizeTrackingPolicy(Qt::Orientations orientations)
{
    d->resizeTrackingPolicy = orientations;
}

Qt::Orientations KexiContextMessageWidget::resizeTrackingPolicy() const
{
    return d->resizeTrackingPolicy;
}

void KexiContextMessageWidget::setPaletteInherited()
{
    if (d->contentsWidget) {
        // fix palette that gets messed after resize
        const QBrush bbrush(backgroundBrush());
        foreach (QWidget* w, findChildren<QWidget*>()) {
            QPalette pal(w->palette());
            pal.setBrush(QPalette::Base, bbrush);
            pal.setBrush(QPalette::Window, bbrush);
            pal.setBrush(QPalette::Button, bbrush);
            w->setPalette(pal);
        }
    }
}

#include "KexiContextMessage.moc"
