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

KexiContextMessage::KexiContextMessage()
 : d(new Private)
{
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
    Private() {}
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
        if (page)
            page->setPalette(origPagePalette);
    }

    QPointer<QWidget> page;
    QPalette origPagePalette;
};

KexiContextMessageWidget::KexiContextMessageWidget(
   QWidget *page, QFormLayout* layout,
   QWidget *context, const KexiContextMessage& message)
 : KMessageWidget()
 , d(new Private)
{
    d->page = page;
    hide();
    int row;
    layout->getWidgetPosition(context, &row, 0);
    setText(message.text());
    setMessageType(KMessageWidget::WarningMessageType);
    setShape(KMessageWidget::RectangleShape);
    setShowCloseButton(false);
    setAutoDelete(true);
    setContentsMargins(3, 0, 3, 0); // to better fit to line edits
    layout->insertRow(row, QString(), this);
    foreach(QAction* action, message.actions()) {
        addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
    }
    if (message.defaultAction()) {
        setDefaultAction(message.defaultAction());
    }
    
    if (d->page) {
        d->setDisabledColorsForPage();
        KexiUtils::installRecursiveEventFilter(d->page, this);
    }
    QTimer::singleShot(10, this, SLOT(animatedShow()));
}

KexiContextMessageWidget::~KexiContextMessageWidget()
{
    d->setEnabledColorsForPage();
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
        return true;
    default:;
    }
//    kDebug() << watched << event->type();
    return KMessageWidget::eventFilter(watched, event);
}

#include "KexiContextMessage.moc"
