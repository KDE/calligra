/* This file is part of the KDE project
   Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiCommandLinkButton.h"
#include <QStylePainter>
#include <QStyleOption>
#include <QTextDocument>
#include <QTextLayout>
#include <QColor>
#include <QFont>
#include <qmath.h>

/*!
    \class KexiCommandLinkButton
    \brief The KexiCommandLinkButton widget provides a Vista style command link button.

    The command link is a new control that was introduced by Windows Vista. It's
    intended use is similar to that of a radio button in that it is used to choose
    between a set of mutually exclusive options. Command link buttons should not
    be used by themselves but rather as an alternative to radio buttons in
    Wizards and dialogs and makes pressing the "next" button redundant.
    The appearance is generally similar to that of a flat pushbutton, but
    it allows for a descriptive text in addition to the normal button text.
    By default it will also carry an arrow icon, indicating that pressing the
    control will open another window or page.

    \sa QPushButton QRadioButton
*/

/*!
    \property KexiCommandLinkButton::description
    \brief A descriptive label to complement the button text

    Setting this property will set a descriptive text on the
    button, complementing the text label. This will usually
    be displayed in a smaller font than the primary text.
*/

/*!
    \property KexiCommandLinkButton::flat
    \brief This property determines whether the button is displayed as a flat
    panel or with a border.

    By default, this property is set to false.

    \sa QPushButton::flat
*/

class KexiCommandLinkButtonPrivate
{
public:
    KexiCommandLinkButtonPrivate(KexiCommandLinkButton *qq)
     : isArrowVisible(false), q(qq) {}

    void init();
    qreal titleSize() const;
    bool usingVistaStyle() const;

    QFont titleFont() const;
    QFont descriptionFont() const;

    QRect titleRect() const;
    QRect descriptionRect() const;

    int textOffset() const;
    int descriptionOffset() const;
    int descriptionHeight(int width) const;
    QColor mergedColors(const QColor &a, const QColor &b, int value) const;

    int topMargin() const { return 10; }
    int leftMargin() const { return 7; }
    int rightMargin() const { return 4; }
    int bottomMargin() const { return 10; }

    QString description;
    QColor currentColor;
    bool isArrowVisible;
    KexiCommandLinkButton *q;
};

// Mix colors a and b with a ratio in the range [0-255]
QColor KexiCommandLinkButtonPrivate::mergedColors(const QColor &a, const QColor &b, int value = 50) const
{
    Q_ASSERT(value >= 0);
    Q_ASSERT(value <= 255);
    QColor tmp = a;
    tmp.setRed((tmp.red() * value) / 255 + (b.red() * (255 - value)) / 255);
    tmp.setGreen((tmp.green() * value) / 255 + (b.green() * (255 - value)) / 255);
    tmp.setBlue((tmp.blue() * value) / 255 + (b.blue() * (255 - value)) / 255);
    return tmp;
}

QFont KexiCommandLinkButtonPrivate::titleFont() const
{
    QFont font = q->font();
    if (usingVistaStyle()) {
        font.setPointSizeF(font.pointSizeF() + 1);
    } else {
        font.setBold(true);
    }

    // Note the font will be resolved against
    // QPainters font, so we need to restore the mask
    int resolve_mask = font.resolve();
    QFont modifiedFont = q->font().resolve(font);
    //modifiedFont.detach();
    modifiedFont.resolve(resolve_mask);
    return modifiedFont;
}

QFont KexiCommandLinkButtonPrivate::descriptionFont() const
{
    return q->font();
}

QRect KexiCommandLinkButtonPrivate::titleRect() const
{
    QRect r = q->rect().adjusted(textOffset(), topMargin(), -rightMargin(), 0);
    if (description.isEmpty())
    {
        QFontMetrics fm(titleFont());
        r.setTop(r.top() + qMax(0, (q->icon().actualSize(q->iconSize()).height()
                 - fm.height()) / 2));
    }

    return r;
}

QRect KexiCommandLinkButtonPrivate::descriptionRect() const
{
    return q->rect().adjusted(textOffset(), descriptionOffset(),
                              -rightMargin(), -bottomMargin());
}

int KexiCommandLinkButtonPrivate::textOffset() const
{
    return q->icon().actualSize(q->iconSize()).width() + leftMargin() + 6;
}

int KexiCommandLinkButtonPrivate::descriptionOffset() const
{
    QFontMetrics fm(titleFont());
    return topMargin() + fm.height();
}

bool KexiCommandLinkButtonPrivate::usingVistaStyle() const
{
    //### This is a hack to detect if we are indeed running Vista style themed and not in classic
    // When we add api to query for this, we should change this implementation to use it.
    return q->style()->inherits("QWindowsVistaStyle")
        && !q->style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
}

void KexiCommandLinkButtonPrivate::init()
{
    q->setAttribute(Qt::WA_Hover);

    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::PushButton);
    policy.setHeightForWidth(true);
    q->setSizePolicy(policy);

    q->setIconSize(QSize(20, 20));
    QStyleOptionButton opt;
    q->initStyleOption(&opt);
    q->setIcon(q->style()->standardIcon(QStyle::SP_CommandLink, &opt));
}

// Calculates the height of the description text based on widget width
int KexiCommandLinkButtonPrivate::descriptionHeight(int widgetWidth) const
{
    // Calc width of actual paragraph
    int lineWidth = widgetWidth - textOffset() - rightMargin();

    qreal descriptionheight = 0;
    if (!description.isEmpty()) {
        QTextLayout layout(description);
        layout.setFont(descriptionFont());
        layout.beginLayout();
        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(lineWidth);
            line.setPosition(QPointF(0, descriptionheight));
            descriptionheight += line.height();
        }
        layout.endLayout();
    }
    return qCeil(descriptionheight);
}

/*!
    \reimp
 */
QSize KexiCommandLinkButton::minimumSizeHint() const
{
    QSize size = sizeHint();
    int minimumHeight = qMax(d->descriptionOffset() + d->bottomMargin(),
                             icon().actualSize(iconSize()).height() + d->topMargin());
    size.setHeight(minimumHeight);
    return size;
}

/*!
    Constructs a command link with no text and a \a parent.
*/

KexiCommandLinkButton::KexiCommandLinkButton(QWidget *parent)
: QPushButton(parent), d(new KexiCommandLinkButtonPrivate(this))
{
    d->init();
}

/*!
    Constructs a command link with the parent \a parent and the text \a
    text.
*/

KexiCommandLinkButton::KexiCommandLinkButton(const QString &text, QWidget *parent)
    : QPushButton(parent), d(new KexiCommandLinkButtonPrivate(this))
{
    setText(text);
    d->init();
}

/*!
    Constructs a command link with a \a text, a \a description, and a \a parent.
*/
KexiCommandLinkButton::KexiCommandLinkButton(const QString &text, const QString &description, QWidget *parent)
    : QPushButton(parent), d(new KexiCommandLinkButtonPrivate(this))
{
    setText(text);
    setDescription(description);
    d->init();
}

/*! \reimp */
bool KexiCommandLinkButton::event(QEvent *e)
{
    return QPushButton::event(e);
}

/*! \reimp */
QSize KexiCommandLinkButton::sizeHint() const
{
//  Standard size hints from UI specs
//  Without note: 135, 41
//  With note: 135, 60

    QSize size = QPushButton::sizeHint();
    QFontMetrics fm(d->titleFont());
    int textWidth = qMax(fm.width(text()), 135);
    int buttonWidth = textWidth + d->textOffset() + d->rightMargin();
    int heightWithoutDescription = d->descriptionOffset() + d->bottomMargin();

    size.setWidth(qMax(size.width(), buttonWidth));
    size.setHeight(qMax(d->description.isEmpty() ? 41 : 60,
                        heightWithoutDescription + d->descriptionHeight(buttonWidth)));
    return size;
}

/*! \reimp */
int KexiCommandLinkButton::heightForWidth(int width) const
{
    int heightWithoutDescription = d->descriptionOffset() + d->bottomMargin();
    // find the width available for the description area
    return qMax(heightWithoutDescription + d->descriptionHeight(width),
                icon().actualSize(iconSize()).height() + d->topMargin() +
                d->bottomMargin());
}

/*! \reimp */
void KexiCommandLinkButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    p.save();

    QStyleOptionButton option;
    initStyleOption(&option);

    //Enable command link appearance on Vista
    option.features |= QStyleOptionButton::CommandLinkButton;
    option.text = QString();
    option.icon = QIcon(); //we draw this ourselves
    QSize pixmapSize = icon().actualSize(iconSize());

    int vOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftVertical) : 0;
    int hOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal) : 0;

    //Draw icon
    p.drawControl(QStyle::CE_PushButton, option);
    if (!icon().isNull())
        p.drawPixmap(d->leftMargin() + hOffset, d->topMargin() + vOffset,
        icon().pixmap(pixmapSize, isEnabled() ? QIcon::Normal : QIcon::Disabled,
                                  isChecked() ? QIcon::On : QIcon::Off));

    //Draw title
    QColor textColor = palette().buttonText().color();
    if (isEnabled() && d->usingVistaStyle()) {
        textColor = QColor(21, 28, 85);
        if (underMouse() && !isDown())
            textColor = QColor(7, 64, 229);
        //A simple text color transition
        d->currentColor = d->mergedColors(textColor, d->currentColor, 60);
        option.palette.setColor(QPalette::ButtonText, d->currentColor);
    }

    int textflags = Qt::TextShowMnemonic;
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
        textflags |= Qt::TextHideMnemonic;

    p.setFont(d->titleFont());
    p.drawItemText(d->titleRect().translated(hOffset, vOffset),
                    textflags, option.palette, isEnabled(), text(), QPalette::ButtonText);

    //Draw description
    textflags |= Qt::TextWordWrap | Qt::ElideRight;
    p.setFont(d->descriptionFont());
    p.drawItemText(d->descriptionRect().translated(hOffset, vOffset), textflags,
                    option.palette, isEnabled(), description(), QPalette::ButtonText);

    //Optional arrow
    if (d->isArrowVisible) {
        const int margin = style()->pixelMetric(QStyle::PM_ButtonMargin, &option, this);
        option.rect.setX(option.rect.width() - margin * 2 - 10);
        style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &option, &p, this);
    }
    p.restore();
}

void KexiCommandLinkButton::setDescription(const QString &description)
{
    d->description = description;
    updateGeometry();
    update();
}

QString KexiCommandLinkButton::description() const
{
    return d->description;
}

bool KexiCommandLinkButton::isArrowVisible() const
{
    return d->isArrowVisible;
}

void KexiCommandLinkButton::setArrowVisible(bool visible)
{
    if (d->isArrowVisible == visible) {
        return;
    }
    d->isArrowVisible = visible;
    update();
}
