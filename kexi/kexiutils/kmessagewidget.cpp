/* This file is part of the KDE libraries
 *
 * Copyright (c) 2011 Aurélien Gâteau <agateau@kde.org>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#include "kmessagewidget.h"
#include "kmessagewidget_p.h"

#include <kaction.h>
#include <kcolorscheme.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kstandardaction.h>

#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QShowEvent>
#include <QTimeLine>
#include <QToolButton>
#include <QPointer>
#include <QPainterPath>
#include <QTransform>

//---------------------------------------------------------------------
// KMessageWidgetFrame
//---------------------------------------------------------------------

KMessageWidgetFrame::KMessageWidgetFrame(QWidget* parent)
 : QFrame(parent), radius(5), m_calloutPointerDirection(KMessageWidget::NoPointer)
{
    const qreal rad = radius;
    m_polyline << QPointF(0, 0)
               << QPointF(rad, rad * 2.0 - 0.5)
               << QPointF(rad * 2.0, 0);
    m_polygon << QPointF(m_polyline[0].x(), m_polyline[0].y() - 1)
              << QPointF(m_polyline[1].x(), m_polyline[1].y() - 1)
              << QPointF(m_polyline[2].x(), m_polyline[2].y() - 1);
}

void KMessageWidgetFrame::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    paintCalloutPointer();
}

KMessageWidget::CalloutPointerDirection KMessageWidgetFrame::calloutPointerDirection() const
{
    return m_calloutPointerDirection;
}

void KMessageWidgetFrame::setCalloutPointerDirection(
    KMessageWidget::CalloutPointerDirection direction)
{
    m_calloutPointerDirection = direction;
    m_calloutPointerTransformation.reset();

    const QSizeF s(size());
    const qreal rad = radius;
    // Original: [v    ]
    //           [     ]
    switch (m_calloutPointerDirection) {
    case KMessageWidget::Up:
        //  ^
        // [    ]
        m_calloutPointerTransformation
            .rotate(180.0)
            .translate(- rad * 5.0 + 0.5, - rad * 2 - 1.5);
        break;
    case KMessageWidget::Down:
        // [    ]
        //  v
        // No rotation needed, this is original position of polyline below
        m_calloutPointerTransformation
            .translate(rad * 3.0 + 0.5, s.height() - rad * 2);
        break;
    case KMessageWidget::Left:
        // <[     ]
        //  [     ]
        m_calloutPointerTransformation
            .rotate(90.0)
            .translate(rad * 1.5 + 0.5, - rad * 2 - 2.5);
        break;
    case KMessageWidget::Right:
        // [     ]>
        // [     ]
        m_calloutPointerTransformation
            .rotate(-90.0)
            .translate(- rad * 3.5, s.width() - rad * 2 - 2.5);
        break;
    default:
        break;
    }
}

void KMessageWidgetFrame::paintCalloutPointer()
{
    if (m_calloutPointerTransformation.isIdentity())
        return;
    QPainter painter(this);
    painter.setTransform(m_calloutPointerTransformation);
    
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(bgBrush.color(), 1.0));
    painter.setBrush(bgBrush);
    painter.drawPolygon(m_polygon);
    painter.setPen(QPen(borderBrush, 1.0));
    painter.drawPolyline(m_polyline);
}

QPoint KMessageWidgetFrame::pointerPosition() const
{
    //kDebug() << "MAPPED:" << t.map(polyline[1]) << mapToGlobal(t.map(polyline[1]).toPoint());
    return m_calloutPointerTransformation.map(m_polyline[1]).toPoint();
}

//---------------------------------------------------------------------
// KMessageWidgetPrivate
//---------------------------------------------------------------------
class KMessageWidgetPrivate
{
public:
    void init(KMessageWidget*);

    KMessageWidget* q;
    KMessageWidgetFrame* content;
    QLabel* iconLabel;
    QLabel* textLabel;
    QToolButton* closeButton;
    QTimeLine* timeLine;

    KMessageWidget::MessageType messageType;
    bool wordWrap;
    QList<QToolButton*> buttons;
    QPixmap contentSnapShot;
    QAction* defaultAction;
    QPointer<QToolButton> defaultButton;
    KColorScheme::ColorSet colorSet;
    KColorScheme::BackgroundRole bgRole;
    KColorScheme::ForegroundRole fgRole;
    bool autoDelete;

    void createLayout();
    void updateSnapShot();
    void updateLayout();
    void slotTimeLineChanged(qreal);
    void slotTimeLineFinished();
    void updateStyleSheet();
};

void KMessageWidgetPrivate::init(KMessageWidget *q_ptr)
{
    q = q_ptr;

    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    timeLine = new QTimeLine(500, q);
    QObject::connect(timeLine, SIGNAL(valueChanged(qreal)), q, SLOT(slotTimeLineChanged(qreal)));
    QObject::connect(timeLine, SIGNAL(finished()), q, SLOT(slotTimeLineFinished()));

    content = new KMessageWidgetFrame(q);
    content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    wordWrap = false;

    iconLabel = new QLabel(content);
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    textLabel = new QLabel(content);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    textLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    textLabel->setContentsMargins(0, 0, 0, 0);
#if 0
    content->setAutoFillBackground(true);
    content->setBackgroundRole(QPalette::Dark);
    textLabel->setAutoFillBackground(true);
    textLabel->setBackgroundRole(QPalette::Mid);
#endif

    KAction* closeAction = KStandardAction::close(q, SLOT(animatedHide()), q);

    closeButton = new QToolButton(content);
    closeButton->setAutoRaise(true);
    closeButton->setDefaultAction(closeAction);
    
    defaultAction = 0;
    autoDelete = false;
    q->setMessageType(KMessageWidget::Information);
}

void KMessageWidgetPrivate::createLayout()
{
    delete content->layout();

    content->resize(q->size());

    qDeleteAll(buttons);
    buttons.clear();

    QList<QToolButton*> buttonsTabOrder;
    Q_FOREACH(QAction* action, q->actions()) {
        QToolButton* button = new QToolButton(content);
        button->setDefaultAction(action);
        button->setFocusPolicy(Qt::StrongFocus);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        buttons.append(button);
        if (defaultAction == action) {
            buttonsTabOrder.prepend(button); // default button is focused first
            q->setFocusProxy(button);
            defaultButton = button;
        }
        else
            buttonsTabOrder.append(button);
    }
    QToolButton *previousButton = 0;
    Q_FOREACH(QToolButton* button, buttonsTabOrder) {
        if (previousButton)
            QWidget::setTabOrder(previousButton, button);
        previousButton = button;
    }

    // Only set autoRaise on if there are no buttons, otherwise the close
    // button looks weird
    closeButton->setAutoRaise(buttons.isEmpty());

    if (wordWrap) {
        QGridLayout* layout = new QGridLayout(content);
        layout->setSpacing(0);
        layout->addWidget(iconLabel, 0, 0);
        layout->addWidget(textLabel, 0, 1);

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        Q_FOREACH(QToolButton* button, buttons) {
            // For some reason, calling show() is necessary here, but not in
            // wordwrap mode
            button->show();
            buttonLayout->addWidget(button);
        }
        buttonLayout->addWidget(closeButton);
        layout->addItem(buttonLayout, 1, 0, 1, 2);
    } else {
        QHBoxLayout* layout = new QHBoxLayout(content);
        layout->addWidget(iconLabel);
        layout->addWidget(textLabel);

        Q_FOREACH(QToolButton* button, buttons) {
            layout->addWidget(button);
        }

        layout->addWidget(closeButton);
    };

    // add margins based on outer margins
    int left, top, right, bottom;
    q->getContentsMargins(&left, &top, &right, &bottom);
    kDebug() << "q->getContentsMargins:" << left << top << right << bottom;
    int add = buttons.isEmpty() ? 2 : (content->radius - 1);
    top *= 2;
    bottom*= 2;
    left *= 2;
    right *= 2;
    add *= 2;
    switch (content->calloutPointerDirection()) {
    case KMessageWidget::Up:
        top += add;
        break;
    case KMessageWidget::Down:
        bottom += add;
        break;
    case KMessageWidget::Left:
        left += add + 4;
        break;
    case KMessageWidget::Right:
        right += add + 4;
        break;
    default:;
    }
    content->layout()->setContentsMargins(
        left, top, right, bottom);

    if (q->isVisible()) {
        if (content->sizeHint().height() >= 0) {
            //q->setFixedHeight(content->sizeHint().height());
            q->setFixedHeight(QWIDGETSIZE_MAX);
        }
    }
    content->updateGeometry();
    q->updateGeometry();
}

void KMessageWidgetPrivate::updateLayout()
{
    if (content->layout()) {
        createLayout();
    }
}

void KMessageWidgetPrivate::updateSnapShot()
{
    contentSnapShot = QPixmap(content->size());
    contentSnapShot.fill(Qt::transparent);
    content->render(&contentSnapShot, QPoint(), QRegion(), QWidget::DrawChildren);
}

void KMessageWidgetPrivate::slotTimeLineChanged(qreal value)
{
    q->setFixedHeight(qMin(value * 2, qreal(1.0)) * content->height());
    q->update();
}

void KMessageWidgetPrivate::slotTimeLineFinished()
{
    if (timeLine->direction() == QTimeLine::Forward) {
        // Show
        content->move(0, 0);
        //q->setFixedHeight(QWIDGETSIZE_MAX);
        if (defaultButton) {
            defaultButton->setFocus();
        }
    } else {
        // Hide
        q->hide();
    }
}

void KMessageWidgetPrivate::updateStyleSheet()
{
    KColorScheme scheme(QPalette::Active, colorSet);
    content->bgBrush = scheme.background(bgRole);
    content->borderBrush = scheme.foreground(fgRole);
    QBrush fg = scheme.foreground();
    int left, top, right, bottom;
    content->getContentsMargins(&left, &top, &right, &bottom);
    kDebug() << "content->getContentsMargins:" << left << top << right << bottom;
    if (!buttons.isEmpty()) {
        //q->setContentsMargins(0, 0, 0, 0);
        content->setContentsMargins(0, 0, 0, 0);
    }
    q->getContentsMargins(&left, &top, &right, &bottom);
    kDebug() << "q->getContentsMargins:" << left << top << right << bottom;
#if 1
    int add = content->radius * 2;
    switch (content->calloutPointerDirection()) {
    case KMessageWidget::Up:
        top += add;
        break;
    case KMessageWidget::Down:
        bottom += add;
        break;
    case KMessageWidget::Left:
        left += add;
        break;
    case KMessageWidget::Right:
        right += add;
        break;
    default:;
    }
    content->setStyleSheet(
        QString(".KMessageWidgetFrame {"
            "background-color: %1;"
            "border-radius: %2px;"
            "margin: %3px %4px %5px %6px;"
            "border: 1px solid %7;"
            "}"
            ".QLabel { color: %8; }"
            )
        .arg(content->bgBrush.color().name())
        .arg(content->radius)
        .arg(top)
        .arg(right)
        .arg(bottom)
        .arg(left)
        .arg(content->borderBrush.color().name())
        .arg(fg.color().name())
    );
#endif
}

//---------------------------------------------------------------------
// KMessageWidget
//---------------------------------------------------------------------
KMessageWidget::KMessageWidget(QWidget* parent)
    : QFrame(parent)
    , d(new KMessageWidgetPrivate)
{
    d->init(this);
}

KMessageWidget::KMessageWidget(const QString& text, QWidget* parent)
    : QFrame(parent)
    , d(new KMessageWidgetPrivate)
{
    d->init(this);
    setText(text);
}

KMessageWidget::~KMessageWidget()
{
    delete d;
}

QString KMessageWidget::text() const
{
    return d->textLabel->text();
}

void KMessageWidget::setText(const QString& text)
{
    d->textLabel->setText(text);
    updateGeometry();
}

KMessageWidget::MessageType KMessageWidget::messageType() const
{
    return d->messageType;
}

void KMessageWidget::setMessageType(KMessageWidget::MessageType type)
{
    d->messageType = type;
    KIcon icon;
    d->colorSet = KColorScheme::Window;
    switch (type) {
    case Positive:
        icon = KIcon("dialog-ok");
        d->bgRole = KColorScheme::PositiveBackground;
        d->fgRole = KColorScheme::PositiveText;
        break;
    case Information:
        icon = KIcon("dialog-information");
        d->bgRole = KColorScheme::NormalBackground;
        d->fgRole = KColorScheme::NormalText;
        d->colorSet = KColorScheme::Tooltip;
        break;
    case Warning:
        icon = KIcon("dialog-warning");
        d->bgRole = KColorScheme::NeutralBackground;
        d->fgRole = KColorScheme::NeutralText;
        break;
    case Error:
        icon = KIcon("dialog-error");
        d->bgRole = KColorScheme::NegativeBackground;
        d->fgRole = KColorScheme::NegativeText;
        break;
    }
    const int size = KIconLoader::global()->currentSize(KIconLoader::MainToolbar);
    d->iconLabel->setPixmap(icon.pixmap(size));

    d->updateStyleSheet();
    d->updateLayout();
}

KMessageWidget::CalloutPointerDirection KMessageWidget::calloutPointerDirection() const
{
    return d->content->calloutPointerDirection();
}

void KMessageWidget::setCalloutPointerDirection(KMessageWidget::CalloutPointerDirection direction)
{
    d->content->setCalloutPointerDirection(direction);
    d->updateStyleSheet();
    d->updateLayout();
}

QSize KMessageWidget::sizeHint() const
{
    ensurePolished();
    kDebug() << "d->content->sizeHint():" << d->content->sizeHint();
    kDebug() << "QFrame::sizeHint():" << QFrame::sizeHint();
    return QFrame::sizeHint();
/*    QSize s1(QFrame::sizeHint());
    QSize s2(d->content->sizeHint());
    return QSize(qMax(s1.width(), s2.width()), qMax(s1.height(), s2.height()));*/
}

QSize KMessageWidget::minimumSizeHint() const
{
    ensurePolished();
    kDebug() << "d->content->minimumSizeHint():" << d->content->minimumSizeHint();
    kDebug() << "QFrame::minimumSizeHint():" << QFrame::minimumSizeHint();
    return QFrame::minimumSizeHint();
/*    QSize s1(QFrame::minimumSizeHint());
    QSize s2(d->content->minimumSizeHint());
    return QSize(qMax(s1.width(), s2.width()), qMax(s1.height(), s2.height()));*/
}

bool KMessageWidget::event(QEvent* event)
{
    if (event->type() == QEvent::Polish && !d->content->layout()) {
        d->createLayout();
    }
    else if (event->type() == QEvent::Hide) {
        if (d->autoDelete) {
            deleteLater();
        }
    }
    return QFrame::event(event);
}

void KMessageWidget::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    if (d->timeLine->state() == QTimeLine::NotRunning) {
        d->content->resize(size());
        d->updateStyleSheet(); // needed because margins could be changed
    }
}

void KMessageWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    if (d->timeLine->state() == QTimeLine::Running) {
        QPainter painter(this);
        painter.setOpacity(d->timeLine->currentValue() * d->timeLine->currentValue());
        painter.drawPixmap(0, 0, d->contentSnapShot);
    }
}

void KMessageWidget::showEvent(QShowEvent* event)
{
    QFrame::showEvent(event);
    if (!event->spontaneous()) {
        int wantedHeight = d->content->sizeHint().height();
#if 0
        d->content->setGeometry(0, 0, width(), wantedHeight);
        if (d->buttons.isEmpty()) {
            setFixedHeight(wantedHeight);
        }
#endif
    }
}

bool KMessageWidget::wordWrap() const
{
    return d->wordWrap;
}

void KMessageWidget::setWordWrap(bool wordWrap)
{
    d->wordWrap = wordWrap;
    d->textLabel->setWordWrap(wordWrap);
    d->updateLayout();
}

bool KMessageWidget::isCloseButtonVisible() const
{
    return d->closeButton->isVisible();
}

void KMessageWidget::setCloseButtonVisible(bool show)
{
    d->closeButton->setVisible(show);
}

void KMessageWidget::addAction(QAction* action)
{
    QFrame::addAction(action);
    d->updateLayout();
}

void KMessageWidget::setDefaultAction(QAction* action)
{
    d->defaultAction = action;
    d->createLayout();
}

void KMessageWidget::removeAction(QAction* action)
{
    QFrame::removeAction(action);
    d->updateLayout();
}

void KMessageWidget::setAutoDelete(bool set)
{
    d->autoDelete = set;
}

void KMessageWidget::animatedShow()
{
    if (!(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects)) {
        show();
        return;
    }

    if (isVisible()) {
        return;
    }

    QFrame::show();
    setFixedHeight(0);
    int wantedHeight = d->content->sizeHint().height();
    d->content->setGeometry(0, -wantedHeight, width(), wantedHeight);

    d->updateSnapShot();

    d->timeLine->setDirection(QTimeLine::Forward);
    if (d->timeLine->state() == QTimeLine::NotRunning) {
        d->timeLine->start();
    }
}

void KMessageWidget::animatedHide()
{
    if (!(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects)) {
        hide();
        return;
    }

    if (!isVisible()) {
        return;
    }

    d->content->move(0, -d->content->height());
    d->updateSnapShot();

    d->timeLine->setDirection(QTimeLine::Backward);
    if (d->timeLine->state() == QTimeLine::NotRunning) {
        d->timeLine->start();
    }
}

void KMessageWidget::setCalloutPointerPosition(const QPoint& globalPos)
{
    move(    
        parentWidget()->mapFromGlobal(
            globalPos - d->content->pos() - d->content->pointerPosition())
    );
}

#include "kmessagewidget.moc"
#include "kmessagewidget_p.moc"
