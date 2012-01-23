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

#include "KexiWelcomeStatusBar.h"
#include <kexiutils/utils.h>

#include <KTextBrowser>
#include <KGlobalSettings>
#include <KIconLoader>
#include <KColorUtils>
#include <KColorScheme>
#include <KStandardDirs>
#include <KDebug>

#include <QLayout>
#include <QProgressBar>
#include <QtCore/qmath.h>

#define HEADER_SIZE 1.0

class KexiStatusTextDocument : public QTextDocument
{
public:
    KexiStatusTextDocument(KexiWelcomeStatusBar* parent) : QTextDocument(parent),
        m_parent(parent)
    {
        // http://www.qtcentre.org/wiki/index.php?title=QTextBrowser_with_images_and_CSS
        qreal fontSize = qFloor(
            (KGlobalSettings::smallestReadableFont().pointSizeF()
             + m_parent->font().pointSizeF())
             / 2.0);
        KColorScheme colorScheme(m_parent->palette().currentColorGroup());
        QString css =
            QString("* { font-size:%1pt; color:%2; } ")
                .arg(fontSize).arg(m_parent->palette().color(QPalette::Text).name())
            + QString(".gray, .gray * { color:%1; } ")
                .arg(KColorUtils::mix(m_parent->palette().color(QPalette::Text),
                                        m_parent->palette().color(QPalette::Base), 0.5).name())
            + "img.icon { vertical-align:bottom; } "
            //+ "body { background-color:yellow; } "
            + "p { text-indent:0px; } "
            + QString("*.head { text-indent:0px; font-size:%1pt; font-weight:bold; } ").arg(
                m_parent->font().pointSizeF() * HEADER_SIZE)
            + QString("a, a * { color:%1; } ")
                .arg(colorScheme.foreground(KColorScheme::LinkText).color().name());
        addResource(QTextDocument::StyleSheetResource, QUrl( "format.css" ), css);
    }
protected:
    virtual QVariant loadResource(int type, const QUrl &name)
    {
        if (type == QTextDocument::ImageResource) {
            if (name.scheme() == QLatin1String("kicon")) {
                return KIconLoader::global()->loadIcon(name.host(), KIconLoader::NoGroup, 16);
            }
            else if (name.scheme() == "feedbackdata") {
                if (name.host() == "progress") {
                    return m_parent->userProgressPixmap();
                }
            }
            else if (name.toString() == "ext") {
                return m_parent->externalLinkPixmap();
            }
        }
        return QTextDocument::loadResource(type, name);
    }
private:
    KexiWelcomeStatusBar *m_parent;
};

//---

class KexiWelcomeStatusBar::Private
{
public:
    Private(KexiWelcomeStatusBar* _q)
     : q(_q)
    {
    }

    KTextBrowser *statusBrowser;
    QProgressBar userProgressBar;
    QVBoxLayout *lyr;
    QPixmap externalLinkPixmap;
private:
    KexiWelcomeStatusBar *q;
};
    
KexiWelcomeStatusBar::KexiWelcomeStatusBar(QWidget* parent)
 : QWidget(parent), d(new Private(this))
{
    d->lyr = new QVBoxLayout(this);
    d->userProgressBar.setTextVisible(false);
    setUserProgress(5);

    init();
}

KexiWelcomeStatusBar::~KexiWelcomeStatusBar()
{
    delete d;
}

void KexiWelcomeStatusBar::init()
{
    QString title("You in Kexi Project"); // Your Status in Kexi Project
    QString html = QString(
        "<p><span class=\"head\">%1</span> <a class='gray' href='feedback://help_your_status'>(?)</a></p>"
        "<p><img src='feedbackdata://progress'> <b>%2%</b> involved</p>"
        "<p><b>Contribute</b> <a class='gray' href='feedback://help_why_contribute'>(Why?)</a></p>"
        "<p><img src=\"kicon://list-add\"> <a href='feedback://show_share_usage_info'>Share Usage Info</a> <span class='gray'>(+5%)</span></p>"
        ).arg(title).arg(d->userProgressBar.value());

    d->statusBrowser = new KTextBrowser;
    d->statusBrowser->setFrameShape(QFrame::NoFrame);
    d->statusBrowser->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
    d->lyr->addWidget(d->statusBrowser);

    KexiStatusTextDocument *doc = new KexiStatusTextDocument(this);
    
    QFont f(d->statusBrowser->font());
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() * HEADER_SIZE);
    d->statusBrowser->setFixedWidth(QFontMetrics(f).width(title + "(?)_____"));
    d->userProgressBar.setFixedSize(
        d->statusBrowser->minimumWidth() - QFontMetrics(f).width("888% involved"),
        fontMetrics().height());
    //statusDoc->addResource(QTextDocument::ImageResource, QUrl( "bg.png" ), QPixmap(":bg"));

    title = "What's New?";
    // <img src='ext'>
    html += QString(
        "<br>"
        "<p><b class=\"head\">%1</b></p>"
        "<p><a href=\"newsdata://0\"> Fruits of CSS2: Office Forms</a></p>"
        "<p align=\"right\"><a href=\"newsdata://show_all\">See all <b>News</b> &raquo;</a></p>"
        ).arg(title);

    title = "Recent Releases";
    html += QString(
        "<br>"
        "<p><b class=\"head\">%1</b></p>"
        "<p><table>"
        "<tr><td><p>Stable release: </p></td><td><a href=\"http://www.koffice.org/news/announcements/koffice-2-3-3-update/\">2.3.3</a></td></tr>"
        "<tr><td><p>Preview release: </p></td><td><a href=\"http://www.calligra.org/news/announcements/calligra-2-4-beta-6/\">2.4 Beta 6</a></td></tr>"
        "</table></p>"
        "<p align=\"right\"><a href=\"newsdata://show_all\">See more <b>Releases</b> &raquo;</a></p>"
        ).arg(title);
    
    QString htmlHead(QLatin1String(
        "<html><head>"
        "<link rel='stylesheet' type='text/css' href='format.css'>"
        "</head><body>"));
    doc->setHtml(htmlHead + html + "</body></html>");
    d->statusBrowser->setDocument(doc);
    kDebug() << d->statusBrowser->toHtml();
}

void KexiWelcomeStatusBar::setUserProgress(int progress)
{
    d->userProgressBar.setValue(progress);
}

QPixmap KexiWelcomeStatusBar::userProgressPixmap()
{
    QPixmap px(d->userProgressBar.size());
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setOpacity(0.5);
    d->userProgressBar.render(&p);
    return px;
}

QPixmap KexiWelcomeStatusBar::externalLinkPixmap()
{
    if (d->externalLinkPixmap.isNull()) {
        d->externalLinkPixmap = QPixmap(
            KStandardDirs::locate("data", "kexi/pics/external-link.png"));
        KColorScheme colorScheme(palette().currentColorGroup());
        QColor c(colorScheme.foreground(KColorScheme::LinkText).color());
        c.setAlpha(100);
        KexiUtils::replaceColors(&d->externalLinkPixmap, c);
    }
    return d->externalLinkPixmap;
}
