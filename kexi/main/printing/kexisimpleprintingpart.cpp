/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexisimpleprintingpart.h"
#include "kexisimpleprintingpagesetup.h"

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KexiMainWindow.h>
#include <core/KexiWindow.h>
#include <core/kexiproject.h>
#include <core/kexipartinfo.h>

KexiSimplePrintingPart::KexiSimplePrintingPart()
        : KexiPart::StaticPart("kexi/simpleprinting", "document-print", i18n("Printing"))
{
    // REGISTERED ID:
//?? m_registeredPartID = (int)KexiPart::QueryObjectType;

    /* m_names["componentName"]
        = i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
        "Use '_' character instead of spaces. First character should be a..z character. "
        "If you cannot use latin characters in your language, use english word.",
        "query");*/
    m_names["instanceCaption"] = i18n("Printing");
    m_supportedViewModes = Kexi::DesignViewMode;
    m_supportedUserViewModes = Kexi::DesignViewMode;
}

KexiSimplePrintingPart::~KexiSimplePrintingPart()
{
}

KexiView* KexiSimplePrintingPart::createView(QWidget *parent, KexiWindow* window,
        KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>* args)
{
    Q_UNUSED(item);
    if (viewMode == Kexi::DesignViewMode) {
        KexiSimplePrintingPageSetup *w = new KexiSimplePrintingPageSetup(
            KexiMainWindow::self(), parent, args);
        return w;
    }

    return 0;
}

#include "kexisimpleprintingpart.moc"
