#ifndef FAKE_kstandardguiitem_H
#define FAKE_kstandardguiitem_H

#include <QPair>
#include <QString>
#include <kguiitem.h>

namespace KStandardGuiItem
{
    enum BidiMode { UseRTL = 0, IgnoreRTL };
    enum StandardItem
    {
        Ok=1, Cancel, Yes, No, Discard, Save, DontSave, SaveAs, Apply, Clear,
        Help, Defaults, Close, Back, Forward, Print, Continue, Open, Quit,
        AdminMode, Reset, Delete, Insert, Configure, Find, Stop, Add, Remove,
        Test, Properties, Overwrite, CloseWindow, CloseDocument
    };

    // static KGuiItem guiItem( StandardItem id );
    // static QString standardItem( StandardItem id );
    static KGuiItem ok() { return KGuiItem(i18n("Ok")); }
    static KGuiItem cancel() { return KGuiItem(i18n("Cancel")); }
    static KGuiItem yes() { return KGuiItem(i18n("Yes")); }
    static KGuiItem no() { return KGuiItem(i18n("No")); }
    static KGuiItem insert() { return KGuiItem(i18n("Insert")); }
    static KGuiItem discard() { return KGuiItem(i18n("Discard")); }
    static KGuiItem save() { return KGuiItem(i18n("Save")); }
    static KGuiItem help() { return KGuiItem(i18n("Help")); }
    static KGuiItem dontSave() { return KGuiItem(i18n("Do not save")); }
    static KGuiItem saveAs() { return KGuiItem(i18n("Save as")); }
    static KGuiItem apply() { return KGuiItem(i18n("Apply")); }
    static KGuiItem clear() { return KGuiItem(i18n("Clear")); }
    static KGuiItem defaults() { return KGuiItem(i18n("Defaults")); }
    static KGuiItem close() { return KGuiItem(i18n("Close")); }
    static KGuiItem closeWindow() { return KGuiItem(i18n("Close Window")); }
    static KGuiItem closeDocument() { return KGuiItem(i18n("Close Document")); }
    static KGuiItem print() { return KGuiItem(i18n("Print")); }
    static KGuiItem properties() { return KGuiItem(i18n("Properties")); }
    static KGuiItem reset() { return KGuiItem(i18n("Reset")); }
    static KGuiItem overwrite() { return KGuiItem(i18n("Overwrite")); }
    static KGuiItem adminMode() { return KGuiItem(i18n("Administrator Mode")); }
    static KGuiItem cont() { return KGuiItem(i18n("Continue")); }
    static KGuiItem del() { return KGuiItem(i18n("Delete")); }
    static KGuiItem open() { return KGuiItem(i18n("Open")); }
    static KGuiItem back( BidiMode useBidi = IgnoreRTL ) { return KGuiItem(i18n("Back")); }
    static KGuiItem forward( BidiMode useBidi = IgnoreRTL ) { return KGuiItem(i18n("Forward")); }
    static KGuiItem configure() { return KGuiItem(i18n("Configure")); }
    static QPair<KGuiItem, KGuiItem> backAndForward() { return QPair<KGuiItem, KGuiItem>(back(),forward()); }
    static KGuiItem quit() { return KGuiItem(i18n("Quit")); }
    static KGuiItem find() { return KGuiItem(i18n("Find")); }
    static KGuiItem stop() { return KGuiItem(i18n("Stop")); }
    static KGuiItem add() { return KGuiItem(i18n("Add")); }
    static KGuiItem remove() { return KGuiItem(i18n("Remove")); }
    static KGuiItem test() { return KGuiItem(i18n("Test")); }

}

#endif
