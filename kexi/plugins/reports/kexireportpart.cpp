/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
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

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KexiView.h>
#include <KexiMainWindowIface.h>
#include "kexiproject.h"
#include <kexipartitem.h>
#include <KexiWindow.h>

#include <kexidb/connection.h>
#include <kexidb/fieldlist.h>
#include <kexidb/field.h>

#include <form.h>
#include <formIO.h>
#include <widgetlibrary.h>

#include <kexiformmanager.h>
#include <kexiformpart.h>

#include "kexireportview.h"
#include "kexireportpart.h"
//Added by qt3to4:
#include <Q3CString>

KFormDesigner::WidgetLibrary* KexiReportPart::static_reportsLibrary = 0L;

KexiReportPart::KexiReportPart(QObject *parent, const QStringList &l)
 : KexiPart::Part((int)KexiPart::ReportObjectType, parent, l)
{
  kexipluginsdbg << "KexiReportPart::KexiReportPart()" << endl;
  setInternalPropertyValue("instanceName",
    i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
    "Use '_' character instead of spaces. First character should be a..z character. "
    "If you cannot use latin characters in your language, use english word.", 
    "report"));
  setInternalPropertyValue("instanceCaption", i18n("Report"));
  setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new report"));
  setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new report."));
  setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode);
  setInternalPropertyValue("newObjectsAreDirty", true);
  
  // Only create form manager if it's not yet created.
  // KexiFormPart could have created is already.
  KFormDesigner::FormManager *formManager = KFormDesigner::FormManager::self();
  if (!formManager) 
    formManager = new KexiFormManager(this, "kexi_form_and_report_manager");

  // Create and store a handle to report' library. Forms will have their own library too.
/* @todo add configuration for supported factory groups */
  QStringList supportedFactoryGroups;
  supportedFactoryGroups += "kexi-report";
  static_reportsLibrary = KFormDesigner::FormManager::createWidgetLibrary(
    formManager, supportedFactoryGroups);
  static_reportsLibrary->setAdvancedPropertiesVisible(false);
}

KexiReportPart::~KexiReportPart()
{
}

KFormDesigner::WidgetLibrary* KexiReportPart::library()
{
  return static_reportsLibrary;
}

void
KexiReportPart::initPartActions()
{
}

void
KexiReportPart::initInstanceActions()
{
  KFormDesigner::FormManager::self()->createActions(
    library(), actionCollectionForMode(Kexi::DesignViewMode), guiClient());
}

KexiWindowData*
KexiReportPart::createWindowData(KexiWindow* window)
{
  return new KexiReportPart::TempData(window);
}

KexiView*
KexiReportPart::createView(QWidget *parent, KexiWindow* window,
  KexiPart::Item &item, Kexi::ViewMode mode, QMap<QString,QVariant>*)
{
  kexipluginsdbg << "KexiReportPart::createView()" << endl;
  KexiMainWindow *win = KexiMainWindowIface::global();
  if (!win || !win->project() || !win->project()->dbConnection())
    return 0;

  KexiReportView *view = new KexiReportView(win, parent, item.name().toLatin1(),
    win->project()->dbConnection() );

  return view;
}

KLocalizedString KexiReportPart::i18nMessage(
  const QString& englishMessage, KexiWindow* window) const
{
  Q_UNUSED(window);
  if (englishMessage=="Design of object \"%1\" has been modified.")
    return ki18n(I18N_NOOP("Design of report \"%1\" has been modified."));
  if (englishMessage=="Object \"%1\" already exists.")
    return ki18n(I18N_NOOP("Report \"%1\" already exists."));
  return Part::i18nMessage(englishMessage, window);
}

//---------------

KexiReportPart::TempData::TempData(QObject* parent)
 : KexiWindowData(parent)
{
}

KexiReportPart::TempData::~TempData()
{
}

#include "kexireportpart.moc"

