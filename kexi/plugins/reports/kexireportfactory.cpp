/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
#include <q3popupmenu.h>
#include <q3valuevector.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3ValueList>
#include <Q3CString>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klineedit.h>

#include <container.h>
#include <form.h>
#include <formmanager.h>
#include <widgetlibrary.h>

#include "reportwidgets.h"
#include "kexireportfactory.h"

KexiReportFactory::KexiReportFactory(QObject *parent, const char *name, const QStringList &)
        : KFormDesigner::WidgetFactory(parent, name)
{
    KFormDesigner::WidgetInfo *wView = new KFormDesigner::WidgetInfo(this);
    wView->setPixmap("report");
    wView->setClassName("KexiReportForm");
    wView->setName(i18n("Report"));
    wView->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "report"));
    wView->setDescription(i18n("A report"));
    addClass(wView);

    KFormDesigner::WidgetInfo *wLabel = new KFormDesigner::WidgetInfo(this);
    wLabel->setPixmap("label");
    wLabel->setClassName("Label");
    wLabel->setName(i18n("Label"));
    wLabel->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "label"));
    wLabel->setDescription(i18n("A label to display text"));
    addClass(wLabel);

    KFormDesigner::WidgetInfo *wPicLabel = new KFormDesigner::WidgetInfo(this);
    wPicLabel->setPixmap("pixmaplabel");
    wPicLabel->setClassName("PicLabel");
    wPicLabel->setName(i18n("Picture Label"));
    wPicLabel->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "picture"));
    wPicLabel->setDescription(i18n("A label to display images or icons"));
    addClass(wPicLabel);

    KFormDesigner::WidgetInfo *wLine = new KFormDesigner::WidgetInfo(this);
    wLine->setPixmap("line");
    wLine->setClassName("ReportLine");
    wLine->setName(i18n("Line"));
    wLine->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "line"));
    wLine->setDescription(i18n("A simple line"));
    addClass(wLine);

    KFormDesigner::WidgetInfo *wSubReport = new KFormDesigner::WidgetInfo(this);
    wSubReport->setPixmap("report");
    wSubReport->setClassName("KexiSubReport");
    wSubReport->setName(i18n("Sub Report"));
    wSubReport->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "subReport"));
    wSubReport->setDescription(i18n("A report embedded in another report"));
    addClass(wSubReport);
}

KexiReportFactory::~KexiReportFactory()
{
}

QString
KexiReportFactory::name()
{
    return "kexireportwidgets";
}

QWidget*
KexiReportFactory::createWidget(const Q3CString &c, QWidget *p, const char *n,
                                KFormDesigner::Container *container, int options)
{
    Q_UNUSED(options);
    kexipluginsdbg << "KexiReportFactory::create() " << this << endl;

    QString text(container->form()->library()->textForWidgetName(n, c));

    if (c == "Label")
        return new Label(text, p, n);
    else if (c == "PicLabel")
        return new PicLabel(DesktopIcon("image-x-generic"), p, n);
    else if (c == "ReportLine")
        return new ReportLine(p, n);
    else if (c == "KexiSubReport")
        return new KexiSubReport(p, n);

    return 0;
}

bool
KexiReportFactory::createMenuActions(const Q3CString &classname, QWidget *w,
                                     Q3PopupMenu *menu, KFormDesigner::Container *container)
{
    Q_UNUSED(w);
    Q_UNUSED(container);
    if (classname == "Label") {
        /*! @todo use KAction */
        menu->insertItem(KIcon("document-properties"), i18n("Edit Rich Text"), this, SLOT(editText()));
        return true;
    }
    return false;
}

bool
KexiReportFactory::startEditing(const Q3CString &c, QWidget *w, KFormDesigner::Container *container)
{
    m_container = container;

    if (c == "Label") {
        QLabel *label = static_cast<QLabel*>(w);
        if (label->textFormat() == RichText) {
            m_widget = w;
            editText();
        } else
            createEditor(c, label->text(), label, container, label->geometry(), label->alignment());
        return true;
    }
    return false;
}

bool
KexiReportFactory::isPropertyVisibleInternal(const Q3CString &classname, QWidget *w, const Q3CString &property, bool isTopLevel)
{
    if (classname == "Label") {
        if (property == "pixmap")
            return false;
    } else if (classname == "PicLabel") {
        if ((property == "text") || (property == "indent") || (property == "textFormat") || (property == "font") || (property == "alignment"))
            return false;
    }

    return WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

Q3ValueList<Q3CString>
KexiReportFactory::autoSaveProperties(const Q3CString &classname)
{
    Q3ValueList<Q3CString> l;

    if (classname == "Label")
        l << "text";
    else if (classname == "PicLabel")
        l << "pixmap";

    return l;
}

/*
void
KexiReportFactory::changeText(const QString &text)
{
  QWidget *w = WidgetFactory::m_widget;
  changeProperty("text", text, m_container);

  int width = w->sizeHint().width();

  if(w->width() < width)
    w->resize(width, w->height() );
}

void
KexiReportFactory::resizeEditor(QWidget *widget, const QCString &)
{
  QSize s = widget->size();
  QPoint p = widget->pos();
  QRect r;

  m_editor->resize(s);
  m_editor->move(p);
}*/

void
KexiReportFactory::editText()
{
    Q3CString classname = m_widget->className();
    QString text;

    if (classname == "Label")
        text = ((QLabel*)m_widget)->text();

    if (editRichText(m_widget, text)) {
        changeProperty("textFormat", "RichText", m_container->form());
        changeProperty("text", text, m_container->form());
    }

    if (classname == "Label")
        m_widget->resize(m_widget->sizeHint());
}

bool
KexiReportFactory::previewWidget(const Q3CString &, QWidget *, KFormDesigner::Container *)
{
    return false;
}

KFORMDESIGNER_WIDGET_FACTORY(KexiReportFactory, kexireportwidgets)

#include "kexireportfactory.moc"

