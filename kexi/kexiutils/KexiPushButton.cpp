/* This file is part of the KDE project
   Copyright (C) 2013 Oleg Kukharchuk <oleg.kuh@gmail.com>

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

#include "KexiPushButton.h"
#include <KLocalizedString>

#include <QDir>
#include <QUrl>

class KexiPushButton::Private
{
public:
    explicit Private(KexiPushButton *qq)
        : hyperlinkType(KexiPushButton::NoHyperlink)
        , hyperlinkTool(KexiUtils::OpenHyperlinkOptions::DefaultHyperlinkTool)
        , executable(false)
        , remote(false)
        , q(qq)
    {
        QObject::connect(q, SIGNAL(clicked()), q, SLOT(slotClicked()));
    }

    QString hyperlink;
    KexiPushButton::HyperlinkType hyperlinkType;
    KexiUtils::OpenHyperlinkOptions::HyperlinkTool hyperlinkTool;
    bool executable;
    bool remote;
    QString basePath;

    KexiPushButton *q;
};

KexiPushButton::KexiPushButton(QWidget *parent)
    : QPushButton(parent)
    , d(new Private(this))
{
}

KexiPushButton::KexiPushButton(const QString &text, QWidget *parent)
    : QPushButton(parent)
    , d(new Private(this))
{
    setText(text);
}

KexiPushButton::~KexiPushButton()
{
    delete d;
}

void KexiPushButton::setHyperlink(const QString &url)
{
    d->hyperlink = url;
}

QString KexiPushButton::hyperlink() const
{
    return d->hyperlink;
}

void KexiPushButton::setHyperlinkType(HyperlinkType type)
{
    d->hyperlinkType = type;
}

KexiPushButton::HyperlinkType KexiPushButton::hyperlinkType() const
{
    return d->hyperlinkType;
}

void KexiPushButton::setHyperlinkTool(KexiUtils::OpenHyperlinkOptions::HyperlinkTool tool)
{
    d->hyperlinkTool = tool;
}

KexiUtils::OpenHyperlinkOptions::HyperlinkTool KexiPushButton::hyperlinkTool() const
{
    return d->hyperlinkTool;
}

void KexiPushButton::setHyperlinkExecutable(bool exec)
{
    d->executable = exec;
}

bool KexiPushButton::isHyperlinkExecutable() const
{
    return d->executable;
}

void KexiPushButton::setRemoteHyperlink(bool remote)
{
    d->remote = remote;
}

bool KexiPushButton::isRemoteHyperlink() const
{
    return d->remote;
}

void KexiPushButton::setLocalBasePath(const QString &basePath)
{
    d->basePath = basePath;
}

void KexiPushButton::slotClicked()
{
    if (d->hyperlinkType == KexiPushButton::NoHyperlink) {
        return;
    }

    QUrl url(d->hyperlink);
    if (d->hyperlinkTool == KexiUtils::OpenHyperlinkOptions::MailerHyperlinkTool
            && url.scheme().isEmpty())
    {
        url.setScheme("mailto");
    }

    if (url.isRelative()) {
        url.setUrl(d->basePath + QDir::separator() + d->hyperlink);
        url.setScheme("file");
    }

    KexiUtils::OpenHyperlinkOptions opt;
    opt.allowExecutable = d->executable;
    opt.allowRemote = d->remote;
    opt.tool = d->hyperlinkTool;
    KexiUtils::openHyperLink(url, this, opt);
}
