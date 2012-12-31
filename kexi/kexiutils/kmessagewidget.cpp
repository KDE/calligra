/* This file is part of the KDE libraries
 *
 * Copyright (c) 2011 Aurélien Gâteau <agateau@kde.org>
 * Copyright (C) 2011-2012 Jarosław Staniek <staniek@kde.org>
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

#include <KexiLinkButton.h>
#include <KoIcon.h>

#include <kaction.h>
#include <kcolorscheme.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kstandardaction.h>
#include <KStandardGuiItem>

#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QShowEvent>
#include <QTimeLine>
#include <QToolButton>
#include <QPointer>
#include <QPainterPath>
#include <QTransform>
#include <QTimer>

#define LAYOUT_SPACING 6

ClickableLabel::ClickableLabel(QWidget *parent)
 : QLabel(parent)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        emit clicked();
    }
    QLabel::mousePressEvent(ev);
}

//---------------------------------------------------------------------
// KMessageWidgetFrame
//---------------------------------------------------------------------

KMessageWidgetFrame::KMessageWidgetFrame(QWidget* parent)
 : QFrame(parent), radius(7),
   m_calloutPointerDirection(KMessageWidget::NoPointer),
   m_sizeForRecentTransformation(-1, -1),
   m_calloutPointerGlobalPosition(-QWIDGETSIZE_MAX, -QWIDGETSIZE_MAX)
{
    const qreal rad = radius;
    m_polyline << QPointF(0, 0)
               << QPointF(0, rad * 2.0) //<< QPointF(rad, rad * 2.0 - 0.5)
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
    m_sizeForRecentTransformation = QSize(-1, -1);
}

void KMessageWidgetFrame::updateCalloutPointerTransformation() const
{
    if (m_sizeForRecentTransformation == parentWidget()->size())
        return;

    m_calloutPointerTransformation.reset();

    const QSizeF s(parentWidget()->size());
    m_sizeForRecentTransformation = parentWidget()->size();
    // kDebug() << size() << parentWidget()->size();
    const qreal rad = radius;
    // Original: [v    ]
    //           [     ]
    switch (m_calloutPointerDirection) {
    case KMessageWidget::Up:
        //  ^
        // [    ]
        m_calloutPointerTransformation
            .rotate(180.0)
            .translate(- rad * 5.0 + 0.5, - rad * 2 - 0.5)
            .scale(-1.0, 1.0);
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
            .translate(rad * 1.5, - rad * 2 - 3.5);
        break;
    case KMessageWidget::Right:
        // [     ]>
        // [     ]
        m_calloutPointerTransformation
            .rotate(-90.0)
            .translate(- rad * 1.5, s.width() - rad * 2 - 3.5)
            .scale(-1.0, 1.0);
        break;
    default:
        break;
    }
}

void KMessageWidgetFrame::setCalloutPointerPosition(const QPoint& globalPos)
{
    m_calloutPointerGlobalPosition = globalPos;
    updateCalloutPointerPosition();
}

QPoint KMessageWidgetFrame::calloutPointerPosition() const
{
    return m_calloutPointerGlobalPosition;
}

void KMessageWidgetFrame::updateCalloutPointerPosition() const
{
    if (m_calloutPointerGlobalPosition == QPoint(-QWIDGETSIZE_MAX, -QWIDGETSIZE_MAX))
        return;
    QWidget *messageWidgetParent = parentWidget()->parentWidget();
    if (messageWidgetParent) {
/*        kDebug() << "m_calloutPointerGlobalPosition:" << m_calloutPointerGlobalPosition
         << "pos():" << pos()
         << "pointerPosition():" << pointerPosition()
         << "(m_calloutPointerGlobalPosition - pos() - pointerPosition()):"
         << (m_calloutPointerGlobalPosition - pos() - pointerPosition())
         << "messageWidgetParent->mapFromGlobal():"
         << messageWidgetParent->mapFromGlobal(
              m_calloutPointerGlobalPosition - pos() - pointerPosition());*/
        parentWidget()->move(
            messageWidgetParent->mapFromGlobal(
                m_calloutPointerGlobalPosition - pos() - pointerPosition())
        );
    }
}

void KMessageWidgetFrame::paintCalloutPointer()
{
    updateCalloutPointerTransformation();

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
    updateCalloutPointerTransformation();
    //kDebug() << "MAPPED:" << t.map(polyline[1]) << mapToGlobal(t.map(polyline[1]).toPoint());
    return m_calloutPointerTransformation.map(m_polyline[1]).toPoint();
}

//---------------------------------------------------------------------
// KMessageWidgetPrivate
//---------------------------------------------------------------------
class KMessageWidgetPrivate
{
public:
    KMessageWidgetPrivate();
    void init(KMessageWidget*);

    KMessageWidget* q;
    KMessageWidgetFrame* content;
    ClickableLabel* iconLabel;
    ClickableLabel* textLabel;
    KexiLinkButton* closeButton;
    QTimeLine* timeLine;

    KMessageWidget::MessageType messageType;
    bool wordWrap;
    QList<QToolButton*> buttons;
    QPixmap contentSnapShot;
    QAction* defaultAction;
    QPointer<QToolButton> defaultButton;
    QSet<QAction*> leftAlignedButtons;
    KColorScheme::ColorSet colorSet;
    KColorScheme::BackgroundRole bgRole;
    KColorScheme::ForegroundRole fgRole;
    bool autoDelete;
    QWidget* contentsWidget;
    bool clickClosesMessage;
    bool resizeToContentsOnTimeLineFinished;

    void createLayout();
    void updateSnapShot();
    void updateLayout();
    void slotTimeLineChanged(qreal);
    void slotTimeLineFinished();
    void updateStyleSheet();
    void tryClickCloseMessage();
};

KMessageWidgetPrivate::KMessageWidgetPrivate()
 : contentsWidget(0)
{
}

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
    resizeToContentsOnTimeLineFinished = false;

    if (contentsWidget) {
        iconLabel = 0;
        textLabel = 0;
    }
    else {
        iconLabel = new ClickableLabel(content);
        iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QObject::connect(iconLabel, SIGNAL(clicked()), q, SLOT(tryClickCloseMessage()));

        textLabel = new ClickableLabel(content);
        textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        textLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
        textLabel->setContentsMargins(0, 0, 0, 0);
        QObject::connect(textLabel, SIGNAL(clicked()), q, SLOT(tryClickCloseMessage()));
#if 0
    content->setAutoFillBackground(true);
    content->setBackgroundRole(QPalette::Dark);
    textLabel->setAutoFillBackground(true);
    textLabel->setBackgroundRole(QPalette::Mid);
#endif
    }
/*    KAction* closeAction = KStandardAction::close(q, SLOT(animatedHide()), q);
    closeButton = new QToolButton(content);
    closeButton->setAutoRaise(true);
    closeButton->setDefaultAction(closeAction);*/
    closeButton = new KexiLinkButton(koIcon("close"), content);
    closeButton->setToolTip(KStandardGuiItem::close().plainText());
    closeButton->setUsesForegroundColor(true);
    QObject::connect(closeButton, SIGNAL(clicked()), q, SLOT(animatedHide()));

    defaultAction = 0;
    autoDelete = false;
    clickClosesMessage = false;
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
    //closeButton->setAutoRaise(buttons.isEmpty());

    if (wordWrap) {
        QGridLayout* layout = new QGridLayout(content);
        layout->setSpacing(LAYOUT_SPACING);
        if (contentsWidget) {
            layout->addItem(new QSpacerItem(1, 6), 0, 0);
            layout->addWidget(contentsWidget, 1, 0, 1, 2);
            layout->addItem(new QSpacerItem(1, 6), 2, 0);

           
/*            if (contentsWidget->maximumWidth() < QWIDGETSIZE_MAX
                && contentsWidget->maximumHeight() < QWIDGETSIZE_MAX
                && contentsWidget->maximumSize() == contentsWidget->minimumSize())
            {
                kDebug() << "contentsWidget->maximumSize():" << contentsWidget->maximumSize();
                kDebug() << "content->size():" << content->size();
                contentsWidget->setFixedSize(
                    contentsWidget->maximumSize() - QSize(120, 0));
                //q->setFixedSize(
                //    contentsWidget->maximumSize() + QSize(100, 0));
                
                kFatal() << contentsWidget->maximumSize();
            }*/
        }
        else {
            layout->addWidget(iconLabel, 0, 0);
            layout->addWidget(textLabel, 0, 1);
        }

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        bool stretchAdded = false;
        Q_FOREACH(QToolButton* button, buttons) {
            if (!stretchAdded && !leftAlignedButtons.contains(button->defaultAction())) {
                buttonLayout->addStretch();
                stretchAdded = true;
            }
            // For some reason, calling show() is necessary here, but not in
            // wordwrap mode
            button->show();
            buttonLayout->addWidget(button);
        }
        if (contentsWidget) {
            buttonLayout->addStretch();
            buttonLayout->addWidget(closeButton);
            buttonLayout->setContentsMargins(0, 10, 0, 0);
            layout->addLayout(buttonLayout, 0, 0);
        }
        else {
            buttonLayout->addWidget(closeButton);
            layout->addItem(buttonLayout, 1, 0, 1, 2);
        }
    } else {
        QHBoxLayout* layout = new QHBoxLayout(content);
        layout->setSpacing(6);
        if (contentsWidget) {
            layout->addWidget(contentsWidget);
        }
        else {
            layout->addWidget(iconLabel);
            layout->addWidget(textLabel);
        }
        Q_FOREACH(QToolButton* button, buttons) {
            layout->addWidget(button);
        }

        layout->addWidget(closeButton);
    };

    // add margins based on outer margins
    int left, top, right, bottom;
    q->getContentsMargins(&left, &top, &right, &bottom);
    kDebug() << "q->getContentsMargins:" << left << top << right << bottom;
    switch (content->calloutPointerDirection()) {
    case KMessageWidget::Up:
        left += 1;
        top += 4;
        bottom += 4;
        right += 2;
        if (!buttons.isEmpty()) {
            top += 4;
            right += 3;
        }
        break;
    case KMessageWidget::Down:
        left += 1;
        top += 4;
        bottom += 4;
        right += 2;
        if (!buttons.isEmpty()) {
            right += 3;
        }
        break;
    case KMessageWidget::Left:
        left += 0;
        top += 3;
        bottom += 3;
        right += 1;
        break;
    case KMessageWidget::Right:
        left += 0;
        top += 3;
        bottom += 3;
        right += 1;
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
    if (!contentsWidget) {
        q->setFixedHeight(qMin(value * 2, qreal(1.0)) * content->height());
    }
    q->update();
}

void KMessageWidgetPrivate::slotTimeLineFinished()
{
    if (timeLine->direction() == QTimeLine::Forward) {
        // Show
        content->move(0, 0);
        content->updateCalloutPointerPosition();
        if (resizeToContentsOnTimeLineFinished) {
            resizeToContentsOnTimeLineFinished = false;
            content->resize(q->size());
            updateStyleSheet(); // needed because margins could be changed
        }
        //q->setFixedHeight(QWIDGETSIZE_MAX);
        if (defaultButton) {
            defaultButton->setFocus();
        }
        emit q->animatedShowFinished();
    } else {
        // Hide
        q->hide();
        emit q->animatedHideFinished();
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
        content->setContentsMargins(6, 0, 0, 0);
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

void KMessageWidgetPrivate::tryClickCloseMessage()
{
    if (clickClosesMessage) {
        QTimer::singleShot(100, q, SLOT(animatedHide()));
    }
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

KMessageWidget::KMessageWidget(QWidget* contentsWidget, QWidget* parent)
    : QFrame(parent)
    , d(new KMessageWidgetPrivate)
{
    d->contentsWidget = contentsWidget;
    d->init(this);
}

KMessageWidget::~KMessageWidget()
{
    delete d;
}

QString KMessageWidget::text() const
{
    return d->textLabel ? d->textLabel->text() : QString();
}

void KMessageWidget::setText(const QString& text)
{
    if (d->textLabel) {
        d->textLabel->setText(text);
        updateGeometry();
    }
}

KMessageWidget::MessageType KMessageWidget::messageType() const
{
    return d->messageType;
}

void KMessageWidget::setMessageType(KMessageWidget::MessageType type)
{
    d->messageType = type;
    KIcon icon;
    d->colorSet = KColorScheme::View;
    switch (type) {
    case Positive:
        icon = koIcon("dialog-ok");
        d->bgRole = KColorScheme::PositiveBackground;
        d->fgRole = KColorScheme::PositiveText;
        break;
    case Information:
        icon = koIcon("dialog-information");
        d->bgRole = KColorScheme::NeutralBackground;
        d->fgRole = KColorScheme::NeutralText;
        break;
    case Warning:
        icon = koIcon("dialog-warning");
        d->bgRole = KColorScheme::NeutralBackground;
        d->fgRole = KColorScheme::NeutralText;
        break;
    case Error:
        icon = koIcon("dialog-error");
        d->bgRole = KColorScheme::NegativeBackground;
        d->fgRole = KColorScheme::NegativeText;
        break;
    }
    if (d->iconLabel) {
        const int size = IconSize(KIconLoader::MainToolbar);
        d->iconLabel->setPixmap(icon.pixmap(size));
    }

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
    d->content->updateCalloutPointerPosition();
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
        //kDebug() << "QEvent::Hide" << event->spontaneous();
        if (!event->spontaneous()) {
            if (d->autoDelete) {
                deleteLater();
            }
        }
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
            d->tryClickCloseMessage();
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
#if 0
        int wantedHeight = d->content->sizeHint().height();
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
    if (d->textLabel) {
        d->textLabel->setWordWrap(wordWrap);
        d->updateLayout();
    }
}

bool KMessageWidget::isCloseButtonVisible() const
{
    return d->closeButton->isVisible();
}

void KMessageWidget::setCloseButtonVisible(bool show)
{
    d->closeButton->setVisible(show);
}

bool KMessageWidget::clickClosesMessage() const
{
    return d->clickClosesMessage;
}

void KMessageWidget::setClickClosesMessage(bool set)
{
    d->clickClosesMessage = set;
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

void KMessageWidget::setButtonLeftAlignedForAction(QAction *action)
{
    d->leftAlignedButtons.insert(action);
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

    d->content->updateCalloutPointerPosition();
    QFrame::show();
    if (d->contentsWidget) {
        int wantedHeight = height();
        d->content->setGeometry(0, 0, width(), wantedHeight);
        setFixedHeight(wantedHeight);
    }
    else {
        setFixedHeight(0);
        int wantedHeight = d->content->sizeHint().height();
        d->content->setGeometry(0, -wantedHeight, width(), wantedHeight);
    }

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
    d->content->setCalloutPointerPosition(globalPos);
}

QPoint KMessageWidget::calloutPointerPosition() const
{
    return d->content->calloutPointerPosition();
}

QBrush KMessageWidget::backgroundBrush() const
{
    return d->content->bgBrush;
}

QBrush KMessageWidget::borderBrush() const
{
    return d->content->borderBrush;
}

void KMessageWidget::resizeToContents()
{
//    kDebug() << LAYOUT_SPACING + d->iconLabel->width() + LAYOUT_SPACING + d->textLabel->width() + LAYOUT_SPACING;
//    kDebug() << "sizeHint():" << sizeHint();
//    kDebug() << "d->content->sizeHint():" << d->content->sizeHint();
    d->resizeToContentsOnTimeLineFinished = true; // try to resize later too if animation in progress
    (void)sizeHint(); // to update d->content->sizeHint()
    setFixedSize(d->content->sizeHint());
}

#include "kmessagewidget.moc"
#include "kmessagewidget_p.moc"
