/*
    Kexi Auto Form Plugin                           
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KexiAutoFormPart.h"
#include "KexiAutoFormView.h"
#include "KexiAutoFormDesignView.h"
#include <KDebug>

KexiAutoFormPart::KexiAutoFormPart(QObject* parent, const QVariantList& args): Part(parent, args)
{
    kDebug();
    setInternalPropertyValue("instanceName",
                             i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                             "Use '_' character instead of spaces. First character should be a..z character. "
                             "If you cannot use latin characters in your language, use english word.",
                             "autoform"));
                             setInternalPropertyValue("instanceCaption", i18n("AutoForm"));
                             setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new AutoForm"));
    setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new autoform."));
    setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode);
    setInternalPropertyValue("newObjectsAreDirty", true);
}

KexiAutoFormPart::~KexiAutoFormPart()
{

}

KexiView* KexiAutoFormPart::createView(QWidget* parent, KexiWindow* window, KexiPart::Item& item, Kexi::ViewMode viewMode, QMap< QString, QVariant >* staticObjectArgs)
{
    Q_UNUSED(window);
    Q_UNUSED(item);
    
    KexiView* view = 0;
    kDebug();
    if (viewMode == Kexi::DataViewMode) {
        //view = new KexiAutoFormView(parent);
        
    } else if (viewMode == Kexi::DesignViewMode) {
        view = new KexiAutoFormDesignView(parent);
    }
    return view;
}

KexiWindowData* KexiAutoFormPart::createWindowData(KexiWindow* window)
{
    kDebug();
    const QString document(loadReport(window->partItem()->name()));
    KexiReportPart::TempData *td = new KexiReportPart::TempData(window);
    
    QDomDocument doc;
    doc.setContent(document);
    
    kDebug() << doc.toString();
    
    QDomElement root = doc.documentElement();
    QDomElement korep = root.firstChildElement("autoform:content");
    QDomElement conn = root.firstChildElement("connection");
    
    td->reportDefinition = korep;
    td->connectionDefinition = conn;
    
    td->name = window->partItem()->name();
    return td;
}

KexiAutoFormPart::TempData::TempData(QObject* parent): KexiWindowData(parent)
{

}
