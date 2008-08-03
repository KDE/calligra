/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "SmallToolButton.h"

#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QActionEvent>
#include <QPointer>
#include <QAction>

#include <KIcon>
#include <KIconLoader>
#include <KGlobalSettings>
#include <KDebug>

#include "utils.h"

//! @internal
class KexiSmallToolButton::Private
{
public:
    Private()
            : enableSlotButtonToggled(true)
            , enableSlotActionToggled(true) {
    }

    QPointer<QAction> action;
bool enableSlotButtonToggled : 1;
bool enableSlotActionToggled : 1;
};

//--------------------------------

KexiSmallToolButton::KexiSmallToolButton(QWidget* parent)
        : QToolButton(parent)
        , d(new Private)
{
    init();
    update(QString(), KIcon());
}

KexiSmallToolButton::KexiSmallToolButton(const QString& text, QWidget* parent)
        : QToolButton(parent)
        , d(new Private)
{
    init();
    update(text, KIcon());
}

KexiSmallToolButton::KexiSmallToolButton(const KIcon& icon, const QString& text,
        QWidget* parent)
        : QToolButton(parent)
        , d(new Private)
{
    init();
    update(text, icon);
}

KexiSmallToolButton::KexiSmallToolButton(QAction* action, QWidget* parent)
        : QToolButton(parent)
        , d(new Private)
{
    d->action = action;
    init();
    if (d->action) {
        connect(d->action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
        update(d->action->text(), d->action->icon(), false);
        setEnabled(d->action->isEnabled());
        setToolTip(d->action->toolTip());
        setWhatsThis(d->action->whatsThis());
        setCheckable(d->action->isCheckable());
        if (d->action->menu()) {
            setPopupMode(QToolButton::InstantPopup); //QToolButton::MenuButtonPopup);
            //setArrowType(Qt::DownArrow);
            //setToolButtonStyle(Qt::ToolButtonTextOnly);
            setMenu(d->action->menu());
        } else {
            connect(this, SIGNAL(toggled(bool)), this, SLOT(slotButtonToggled(bool)));
            connect(d->action, SIGNAL(toggled(bool)), this, SLOT(slotActionToggled(bool)));
        }
    }
    connect(this, SIGNAL(clicked()), action, SLOT(trigger()));
// connect(action, SIGNAL(enabled(bool)), this, SLOT(setEnabled(bool)));
    updateAction();
}

KexiSmallToolButton::~KexiSmallToolButton()
{
    delete d;
}

void KexiSmallToolButton::updateAction()
{
// setDefaultAction(0);
// setDefaultAction(d->action);

    /*
      if (!d->action)
        return;
      removeAction(d->action);
      addAction(d->action);
      update(d->action->text(), KIcon(d->action->icon()));
      setShortcut(d->action->shortcut());
      setToolTip( d->action->toolTip());
      setWhatsThis( d->action->whatsThis());*/
}

void KexiSmallToolButton::init()
{
// QPalette palette(this->palette());
//  palette.setColor(backgroundRole(), ??);
// setPalette(palette);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QFont f(KGlobalSettings::toolBarFont());
    f.setPixelSize(KexiUtils::smallFont().pixelSize());
    setFont(f);
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void KexiSmallToolButton::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    QToolButton::setToolButtonStyle(style);
    update(text(), icon(), false);
}

void KexiSmallToolButton::update(const QString& text, const QIcon& icon, bool tipToo)
{
    int width = 0;
    if (text.isEmpty() || toolButtonStyle() == Qt::ToolButtonIconOnly) {
        width = 10;
//  setToolButtonStyle(Qt::ToolButtonIconOnly);
    } else {
        QFont f(KGlobalSettings::toolBarFont());
        f.setPixelSize(KexiUtils::smallFont().pixelSize());
        width += QFontMetrics(f).width(text + " ");
        if (toolButtonStyle() != Qt::ToolButtonTextOnly)
            QToolButton::setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        QToolButton::setText(text);
        if (tipToo)
            setToolTip(text);
    }
    if (toolButtonStyle() == Qt::ToolButtonTextOnly) {
        QToolButton::setIcon(QIcon());
    } else if (!icon.isNull()) {
        width += IconSize(KIconLoader::Small);
        QToolButton::setIcon(icon);
    }
    QStyleOption opt;
    width += style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);
    //setFixedWidth( width );
}

QSize KexiSmallToolButton::sizeHint() const
{
    return QToolButton::sizeHint()
           - QSize((toolButtonStyle() == Qt::ToolButtonTextBesideIcon) ? 4 : 0, 0);
}

void KexiSmallToolButton::setIcon(const QIcon& icon)
{
    update(text(), icon);
}

void KexiSmallToolButton::setIcon(const QString& icon)
{
    setIcon(KIcon(icon));
}

void KexiSmallToolButton::setText(const QString& text)
{
    update(text, KIcon(icon()));
}

#if 0
void KexiSmallToolButton::paintEvent(QPaintEvent *pe)
{
#ifdef __GNUC__
#warning TODO KexiSmallToolButton::drawButton() - painting OK?
#endif
    QToolButton::paintEvent(pe);
    /* QPainter painter(this);
      if (QToolButton::menu()) {
        QStyle::State arrowFlags = QStyle::State_None;
        QStyleOption option;
        option.initFrom(this);
        if (isDown())
          option.state |= QStyle::State_DownArrow;
        if (isEnabled())
          option.state |= QStyle::State_Enabled;
        style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &option, &painter, this);
      }*/
}
#endif

void KexiSmallToolButton::slotActionChanged()
{
    kDebug() << "slotActionChanged()" << d->action->isEnabled() << endl;
    setEnabled(d->action->isEnabled());
}

void KexiSmallToolButton::slotButtonToggled(bool checked)
{
    if (!d->enableSlotButtonToggled)
        return;
    QObject *view = KexiUtils::findParent<QObject*>(this, "KexiView");
    kDebug() << QString("checked=%1 action=%2 view=%3")
    .arg(checked).arg(d->action ? d->action->text() : QString())
    .arg(view ? view->objectName() : QString("??"));
    d->enableSlotActionToggled = false;
// if (d->action)
//  d->action->setChecked(checked);
    d->enableSlotActionToggled = true;
}

void KexiSmallToolButton::slotActionToggled(bool checked)
{
    if (!d->enableSlotActionToggled)
        return;
    QObject *view = KexiUtils::findParent<QObject*>(this, "KexiView");
    kDebug() << QString("checked=%1 action=%2 view=%3")
    .arg(checked).arg(d->action ? d->action->text() : QString())
    .arg(view ? view->objectName() : QString("??"));
    d->enableSlotButtonToggled = false;
    setChecked(checked);
    d->enableSlotButtonToggled = true;
}

QAction* KexiSmallToolButton::action() const
{
    return d->action;
}

//------------------------------------------

KexiToolBarSeparator::KexiToolBarSeparator(QWidget *parent)
        : QWidget(parent)
        , m_orientation(Qt::Horizontal)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//! @todo
    setFixedHeight(parent->height() - 6);
}

KexiToolBarSeparator::~KexiToolBarSeparator()
{
}

void KexiToolBarSeparator::initStyleOption(QStyleOption *o) const
{
    o->initFrom(this);
    if (orientation() == Qt::Horizontal)
        o->state |= QStyle::State_Horizontal;
}

void KexiToolBarSeparator::setOrientation(Qt::Orientation o)
{
    m_orientation = o;
    update();
}

Qt::Orientation KexiToolBarSeparator::orientation() const
{
    return m_orientation;
}

QSize KexiToolBarSeparator::sizeHint() const
{
    QStyleOption o;
    initStyleOption(&o);
    const int sepExtent = style()->pixelMetric(
                              QStyle::PM_ToolBarSeparatorExtent, &o, 0);//parentWidget());
    return QSize(sepExtent, sepExtent);//height());
}

void KexiToolBarSeparator::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOption o;
    initStyleOption(&o);
// o.rect.setHeight( o.rect.height()-6 );
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &o, &p, parentWidget());
}

#include "SmallToolButton.moc"
