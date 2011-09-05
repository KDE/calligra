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

#include "KexiLinkWidget.h"

#include <QEvent>

#include <KColorScheme>
#include <KDebug>

class KexiLinkWidget::Private
{
public:
    Private(KexiLinkWidget* qq) : q(qq) {
        q->setFocusPolicy(Qt::StrongFocus);
        q->setTextFormat(Qt::RichText);
        updateColors();
    }

    void updateColors() {
        KColorScheme scheme(q->palette().currentColorGroup());
        linkColor = scheme.foreground(KColorScheme::LinkText).color();
    }

    void updateText() {
        QString text;
        text = QString("<a href=\"%1\" style=\"color:%2;\">%3</a>")
            .arg(link).arg(linkColor.name()).arg(linkText);
        if (!format.isEmpty()) {
            text = QString(format).replace("%L", text);
        }
        q->setText(text);
    }

    KexiLinkWidget * const q;
    QString link;
    QString linkText;
    QString format;
    QColor linkColor;
};

KexiLinkWidget::KexiLinkWidget(QWidget* parent)
 : QLabel(parent), d(new Private(this))
{
}

KexiLinkWidget::KexiLinkWidget(
    const QString& link, const QString& linkText, QWidget* parent)
 : QLabel(parent), d(new Private(this))
{
    d->link = link;
    d->linkText = linkText;
    d->updateText();
}

KexiLinkWidget::~KexiLinkWidget()
{
    delete d;
}

QString KexiLinkWidget::link() const
{
    return d->link;
}

void KexiLinkWidget::setLink(const QString& link)
{
    d->link = link;
    d->updateText();
}

QString KexiLinkWidget::linkText() const
{
    return d->linkText;
}

void KexiLinkWidget::setLinkText(const QString& linkText)
{
    d->linkText = linkText;
    d->updateText();
}

QString KexiLinkWidget::format() const
{
    return d->format;
}

void KexiLinkWidget::setFormat(const QString& format)
{
    d->format = format;
    d->updateText();
}

void KexiLinkWidget::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::PaletteChange:
        d->updateColors();
        d->updateText();
        break;
    default:;
    }
    QLabel::changeEvent(event);
}

#include "KexiLinkWidget.moc"
