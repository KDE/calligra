/* This file is part of the KDE libraries
   Copyright (C) 2001 Holger Freyther <freyther@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kstandardguiitem.h"

#include <QApplication>

#include <kguiitem.h>

static const QString i18n(const char* a) { return QApplication::translate("KStandardGuiItem", a); }
static const QString i18nc(const char* comment, const char* a) { return QApplication::translate("KStandardGuiItem", a, comment); }

namespace KStandardGuiItem
{

KGuiItem guiItem( StandardItem ui_enum )
{
  switch (ui_enum ) {
  case Ok       : return ok();
  case Cancel   : return cancel();
  case Yes      : return yes();
  case No       : return no();
  case Discard  : return discard();
  case Save     : return save();
  case DontSave : return dontSave();
  case SaveAs   : return saveAs();
  case Apply    : return apply();
  case Clear    : return clear();
  case Help     : return help();
  case Close    : return close();
  case CloseWindow : return closeWindow();
  case CloseDocument : return closeDocument();
  case Defaults : return defaults();
  case Back     : return back();
  case Forward  : return forward();
  case Print    : return print();
  case Continue : return cont();
  case Open     : return open();
  case Quit     : return quit();
  case AdminMode: return adminMode();
  case Reset    : return reset();
  case Delete   : return del();
  case Insert   : return insert();
  case Configure: return configure();
  case Find     : return find();
  case Stop     : return stop();
  case Add      : return add();
  case Remove   : return remove();
  case Test     : return test();
  case Properties : return properties();
  case Overwrite : return overwrite();
  default       : return KGuiItem();
  };
}

QString standardItem( StandardItem ui_enum )
{
  switch (ui_enum ) {
  case Ok       : return QLatin1String("ok");
  case Cancel   : return QLatin1String("cancel");
  case Yes      : return QLatin1String("yes");
  case No       : return QLatin1String("no");
  case Discard  : return QLatin1String("discard");
  case Save     : return QLatin1String("save");
  case DontSave : return QLatin1String("dontSave");
  case SaveAs   : return QLatin1String("saveAs");
  case Apply    : return QLatin1String("apply");
  case Help     : return QLatin1String("help");
  case Close    : return QLatin1String("close");
  case CloseWindow : return QLatin1String("closeWindow");
  case CloseDocument : return QLatin1String("closeDocument");
  case Defaults : return QLatin1String("defaults");
  case Back     : return QLatin1String("back");
  case Forward  : return QLatin1String("forward");
  case Print    : return QLatin1String("print");
  case Continue : return QLatin1String("continue");
  case Open     : return QLatin1String("open");
  case Quit     : return QLatin1String("quit");
  case AdminMode: return QLatin1String("adminMode");
  case Delete   : return QLatin1String("delete");
  case Insert   : return QLatin1String("insert");
  case Configure: return QLatin1String("configure");
  case Find     : return QLatin1String("find");
  case Stop     : return QLatin1String("stop");
  case Add      : return QLatin1String("add");
  case Remove   : return QLatin1String("remove");
  case Test     : return QLatin1String("test");
  case Properties : return QLatin1String("properties");
  case Overwrite : return QLatin1String("overwrite");
  default       : return QString();
  };
}

KGuiItem ok()
{
  return KGuiItem( i18n( "&OK" ), QStringLiteral("dialog-ok") );
}


KGuiItem cancel()
{
  return KGuiItem( i18n( "&Cancel" ), QStringLiteral("dialog-cancel") );
}

KGuiItem yes()
{
  return KGuiItem( i18n( "&Yes" ), QStringLiteral("dialog-ok"), i18n( "Yes" ) );
}

KGuiItem no()
{
  return KGuiItem( i18n( "&No" ), QStringLiteral("process-stop"), i18n( "No" ) );
}

KGuiItem discard()
{
  return KGuiItem( i18n( "&Discard" ), QStringLiteral("edit-clear"), i18n( "Discard changes" ),
                   i18n( "Pressing this button will discard all recent "
                         "changes made in this dialog." ) );
}

KGuiItem save()
{
  return KGuiItem( i18n( "&Save" ), QStringLiteral("document-save"), i18n( "Save data" ) );
}

KGuiItem dontSave()
{
  return KGuiItem( i18n( "&Do Not Save" ), QStringLiteral(""),
                   i18n( "Do not save data" ) );
}

KGuiItem saveAs()
{
  return KGuiItem( i18n( "Save &As..." ), QStringLiteral("document-save-as"),
                   i18n( "Save file with another name" ) );
}

KGuiItem apply()
{
  return KGuiItem( i18n( "&Apply" ), QStringLiteral("dialog-ok-apply"), i18n( "Apply changes" ),
                   i18n( "When you click <b>Apply</b>, the settings will be "
                         "handed over to the program, but the dialog "
                         "will not be closed.\n"
                         "Use this to try different settings." ) );
}

KGuiItem adminMode()
{
  return KGuiItem( i18n( "Administrator &Mode..." ), QStringLiteral(""), i18n( "Enter Administrator Mode" ),
                   i18n( "When you click <b>Administrator Mode</b> you will be prompted "
                         "for the administrator (root) password in order to make changes "
                         "which require root privileges." ) );
}

KGuiItem clear()
{
  return KGuiItem( i18n( "C&lear" ), QStringLiteral("edit-clear"),
                   i18n( "Clear input" ),
                   i18n( "Clear the input in the edit field" ) );
}

KGuiItem help()
{
  return KGuiItem( i18nc( "show help", "&Help" ), QStringLiteral("help-contents"),
                   i18n( "Show help" ) );
}

KGuiItem close()
{
  return KGuiItem( i18n( "&Close" ), QStringLiteral("window-close"),
                   i18n( "Close the current window or document" ) );
}

KGuiItem closeWindow()
{
  return KGuiItem( i18n( "&Close Window" ), QStringLiteral("window-close"),
		   i18n( "Close the current window." ) );
}

KGuiItem closeDocument()
{
  return KGuiItem( i18n( "&Close Document" ), QStringLiteral("document-close"),
		   i18n( "Close the current document." ) );
}

KGuiItem defaults()
{
  return KGuiItem( i18n( "&Defaults" ), QStringLiteral("document-revert"),
                   i18n( "Reset all items to their default values" ) );
}

KGuiItem back( BidiMode useBidi )
{
  QString icon = ( useBidi == UseRTL && QApplication::isRightToLeft() )
                 ? QStringLiteral("go-next") : QStringLiteral("go-previous");
  return KGuiItem( i18nc( "go back", "&Back" ), icon,
                   i18n( "Go back one step" ) );
}

KGuiItem forward( BidiMode useBidi )
{
  QString icon = ( useBidi == UseRTL && QApplication::isRightToLeft() )
                 ? QStringLiteral("go-previous") : QStringLiteral("go-next");
  return KGuiItem( i18nc( "go forward", "&Forward" ), icon,
                   i18n( "Go forward one step" ) );
}

QPair<KGuiItem, KGuiItem> backAndForward()
{
  return qMakePair( back( UseRTL ), forward( UseRTL ) );
}

KGuiItem print()
{
  return KGuiItem( i18n( "&Print..." ), QStringLiteral("document-print"),
                   i18n( "Opens the print dialog to print "
                         "the current document" ) );
}

KGuiItem cont()
{
  return KGuiItem( i18n( "C&ontinue" ), QStringLiteral("arrow-right"),
                   i18n( "Continue operation" ) );
}

KGuiItem del()
{
  return KGuiItem( i18n( "&Delete" ), QStringLiteral("edit-delete"),
                   i18n( "Delete item(s)" ) );
}

KGuiItem open()
{
  return KGuiItem( i18n( "&Open..." ), QStringLiteral("document-open"),
                   i18n( "Open file" ) );
}

KGuiItem quit()
{
  return KGuiItem( i18n( "&Quit" ), QStringLiteral("application-exit"),
                   i18n( "Quit application" ) );
}

KGuiItem reset()
{
  return KGuiItem( i18n( "&Reset" ), QStringLiteral("edit-undo"),
                  i18n( "Reset configuration" ) );
}

KGuiItem insert()
{
  return KGuiItem( i18nc( "Verb", "&Insert" ) );
}

KGuiItem configure()
{
  return KGuiItem( i18n( "Confi&gure..." ), QStringLiteral("configure") );
}

KGuiItem find()
{
  return KGuiItem(i18n("&Find"), QStringLiteral("edit-find"));
}

KGuiItem stop()
{
  return KGuiItem(i18n("Stop"), QStringLiteral("process-stop"));
}

KGuiItem add()
{
  return KGuiItem(i18n("Add"), QStringLiteral("list-add"));
}

KGuiItem remove()
{
  return KGuiItem(i18n("Remove"), QStringLiteral("list-remove"));
}

KGuiItem test()
{
  return KGuiItem(i18n("Test"));
}

KGuiItem properties()
{
  return KGuiItem(i18n("Properties"), QStringLiteral("document-properties"));
}

KGuiItem overwrite()
{
  return KGuiItem(i18n("&Overwrite"));
}

void assign(QPushButton *button, StandardItem item)
{
    KGuiItem::assign(button, KStandardGuiItem::guiItem(item));
}

} // KStandardGuiItem namespace

