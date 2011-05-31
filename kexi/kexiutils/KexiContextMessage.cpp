/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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
#include <QEvent>

#include <kexiutils/utils.h>
#include "KexiAssistantPage.h"
#include "KexiLinkWidget.h"

#include <KDebug>

class KexiContextMessage::Private
{
public:
    Private() : defaultAction(0) {}
    ~Private() {
//        qDeleteAll(actions);
    }
    QString text;
    QList<QAction*> actions;
    QAction* defaultAction;
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

void KexiContextMessage::addAction(QAction* action)
{
    d->actions.append(action);
}
    
QList<QAction*> KexiContextMessage::actions() const
{
    return d->actions;
}

void KexiContextMessage::setDefaultAction(QAction* action)
{
    d->defaultAction = action;
}

QAction* KexiContextMessage::defaultAction() const
{
    return d->defaultAction;
}

// ----

class KexiContextMessageWidget::Private
{
public:
    Private() : hasActions(false), eventBlocking(true) {}
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
        if (page && hasActions)
            page->setPalette(origPagePalette);
    }

    QPointer<QWidget> page;
    QList< QPointer<QWidget> > enabledLinks;
    QPalette origPagePalette;
    QPointer<QWidget> context;
    QPointer<QWidget> nextFocusWidget;
    bool hasActions;
    bool eventBlocking;
};

KexiContextMessageWidget::KexiContextMessageWidget(
    QWidget *page, QFormLayout* layout,
    QWidget *context, const KexiContextMessage& message)
 : KMessageWidget()
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
    init(0, layout, context, KexiContextMessage(message));
}

void KexiContextMessageWidget::init(
    QWidget *page, QFormLayout* layout,
    QWidget *context, const KexiContextMessage& message)
{
    d->context = context;
    d->page = page;
    hide();
    setText(message.text());
    setMessageType(KMessageWidget::Warning);
    setWordWrap(true);
    setCloseButtonVisible(false);
    setAutoDelete(true);
    setContentsMargins(3, 0, 3, 0); // to better fit to line edits
    d->hasActions = !message.actions().isEmpty();
    if (d->page && d->hasActions) {
        d->setDisabledColorsForPage();
        foreach (KexiLinkWidget* w, d->page->findChildren<KexiLinkWidget*>()) {
            kDebug() << w << w->isEnabled();
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
        setCalloutPointerDirection(KMessageWidget::Left);
    }
    else {
        if (d->page) {
            setParent(page);
        }
    }

    if (d->hasActions) {
        foreach(QAction* action, message.actions()) {
            KMessageWidget::addAction(action);
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

#include "KexiContextMessage.moc"
