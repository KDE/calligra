/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
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

#include "kexitableviewheader.h"

#include <QApplication>
#include <QToolTip>
#include <QStyle>
#include <QStyleOptionHeader>
#include <QMouseEvent>
#include <QHelpEvent>

#include <KIconLoader>
#include <kexiutils/utils.h>
#include <kexiutils/styleproxy.h>

//! @internal A style that allows to temporary change background color while
//!           drawing header section primitive. Used in KexiTableViewHeader.
class KexiTableViewHeaderStyle : public KexiUtils::StyleProxy
{
public:
    KexiTableViewHeaderStyle(QStyle *parentStyle, QWidget *widget)
            : KexiUtils::StyleProxy(parentStyle) {
        setBackgroundColor(widget->palette().color(widget->backgroundRole()));
    }
    virtual ~KexiTableViewHeaderStyle() {}

    virtual void drawControl(ControlElement ce,
                             const QStyleOption * option, QPainter * painter, const QWidget * widget = 0) const
    //drawPrimitive
    //PrimitiveElement pe,
    //QPainter *p, const QRect &r, const QColorGroup &cg, SFlags flags = Style_Default,
    //const QStyleOption& option = QStyleOption::Default ) const
    {
        if (ce == CE_Header/*CE_HeaderSection*/ && option) {
            QStyleOptionHeader newOption(*qstyleoption_cast<const QStyleOptionHeader*>(option));
            newOption.palette.setColor(QPalette::Button, m_backgroundColor);
            //set background color as well (e.g. for thinkeramik)
            newOption.palette.setColor(QPalette::Window, m_backgroundColor);
            StyleProxy::drawControl(ce, &newOption, painter, widget);
            return;
        }
        StyleProxy::drawControl(ce, option, painter, widget);
    }

    void setBackgroundColor(const QColor& color) {
        m_backgroundColor = color;
    }

protected:
    QColor m_backgroundColor;
};

KexiTableViewHeader::KexiTableViewHeader(QWidget * parent)
        : Q3Header(parent)
        , m_lastToolTipSection(-1)
        , m_selectionBackgroundColor(qApp->palette().highlight().color())
        , m_selectedSection(-1)
        , m_styleChangeEnabled(true)
{
    styleChanged();
    installEventFilter(this);
    connect(this, SIGNAL(sizeChange(int, int, int)),
            this, SLOT(slotSizeChange(int, int, int)));
}

KexiTableViewHeader::~KexiTableViewHeader()
{
    if (m_privateStyle) {
        setStyle(0);
        //delete (QObject*)m_privateStyle;
    }
}

bool KexiTableViewHeader::event(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        styleChanged();
    return Q3Header::event(event);
}

void KexiTableViewHeader::styleChanged()
{
    if (!m_styleChangeEnabled)
        return;
    m_styleChangeEnabled = false;
    if (m_privateStyle) {
        setStyle(0);
        //delete (QObject*)m_privateStyle;
    }
    setStyle(m_privateStyle = new KexiTableViewHeaderStyle(style(), this));
    m_styleChangeEnabled = true;
}

int KexiTableViewHeader::addLabel(const QString & s, int size)
{
    m_toolTips += "";
    slotSizeChange(0, 0, 0);//refresh
    return Q3Header::addLabel(s, size);
}

int KexiTableViewHeader::addLabel(const QIcon & icon, const QString & s, int size)
{
    m_toolTips += "";
    slotSizeChange(0, 0, 0);//refresh
    return Q3Header::addLabel(icon, s, size);
}

void KexiTableViewHeader::removeLabel(int section)
{
    if (section < 0 || section >= count())
        return;
    QStringList::Iterator it = m_toolTips.begin();
    if (it!=m_toolTips.end()) {
        it += section;
        it = m_toolTips.erase(it);
    }
    slotSizeChange(0, 0, 0);//refresh
    Q3Header::removeLabel(section);
}

void KexiTableViewHeader::setToolTip(int section, const QString & toolTip)
{
    if (section < 0 || section >= (int)m_toolTips.count())
        return;
    m_toolTips[ section ] = toolTip;
}

bool KexiTableViewHeader::eventFilter(QObject * watched, QEvent * e)
{
    if (e->type() == QEvent::MouseMove) {
        const int section = sectionAt(static_cast<QMouseEvent*>(e)->x());
        if (section != m_lastToolTipSection && section >= 0 && section < (int)m_toolTips.count()) {
            //QToolTip::remove(this, m_toolTipRect);
#ifdef __GNUC__
#warning TODO KexiTableViewHeader::eventFilter
#else
#pragma WARNING( TODO KexiTableViewHeader::eventFilter  )
#endif
            QString tip = m_toolTips[ section ];
            if (tip.isEmpty()) { //try label
                QFontMetrics fm(font());
                int minWidth = fm.width(label(section))
                               + style()->pixelMetric(QStyle::PM_HeaderMargin);
                QIcon *iset = iconSet(section);
                if (iset)
                    minWidth += (2 + iset->pixmap(IconSize(KIconLoader::Small)).width()); //taken from QHeader::sectionSizeHint()
                if (minWidth > sectionSize(section))
                    tip = label(section);
            }
            if (tip.isEmpty()) {
                m_lastToolTipSection = -1;
            } else {
#ifdef __GNUC__
#warning QToolTip::showText() OK?
#else
#pragma WARNING( QToolTip::showText() OK? )
#endif
                QToolTip::showText(static_cast<QMouseEvent*>(e)->globalPos(), tip,
                                   this, m_toolTipRect = sectionRect(section));
                m_lastToolTipSection = section;
            }
        }
    } else if (e->type() == QEvent::ToolTip) {
//        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
#ifdef __GNUC__
#warning TODO
#else
#pragma WARNING( TODO )
#endif
    }
//   if (e->type()==QEvent::MouseButtonPress) {
// todo
//   }
    return Q3Header::eventFilter(watched, e);
}

void KexiTableViewHeader::slotSizeChange(int /*section*/, int /*oldSize*/, int /*newSize*/)
{
    if (m_lastToolTipSection > 0)
        QToolTip::hideText();
#ifdef __GNUC__
#warning TODO OK?
#else
#pragma WARNING( TODO OK? )
#endif
//  QToolTip::remove(this, m_toolTipRect);
    m_lastToolTipSection = -1; //tooltip's rect is now invalid
}

void KexiTableViewHeader::setSelectionBackgroundColor(const QColor &color)
{
    m_selectionBackgroundColor = color;
}

QColor KexiTableViewHeader::selectionBackgroundColor() const
{
    return m_selectionBackgroundColor;
}

void KexiTableViewHeader::setSelectedSection(int section)
{
    if (m_selectedSection == section || (section != -1 && section >= count()))
        return;
    const int oldSection = m_selectedSection;
    m_selectedSection = section;
    if (oldSection != -1)
        update(sRect(oldSection));
    if (m_selectedSection != -1)
        update(sRect(m_selectedSection));
}

int KexiTableViewHeader::selectedSection() const
{
    return m_selectedSection;
}

void KexiTableViewHeader::paintSection(QPainter * p, int index, const QRect & fr)
{
    const bool paintSelection = index == m_selectedSection && index != -1;
    if (paintSelection && dynamic_cast<KexiTableViewHeaderStyle*>(style())) {
//! @todo Qt4: blend entire QBrush?
        dynamic_cast<KexiTableViewHeaderStyle*>(style())->setBackgroundColor(
            KexiUtils::blendedColors(
                palette().color(backgroundRole()), m_selectionBackgroundColor, 2, 1));
    }

    Q3Header::paintSection(p, index, fr);

    if (paintSelection && dynamic_cast<KexiTableViewHeaderStyle*>(style())) {
        //revert the color for subsequent paints
        dynamic_cast<KexiTableViewHeaderStyle*>(style())->setBackgroundColor(
            palette().color(backgroundRole()));
    }
}

#include "kexitableviewheader.moc"
