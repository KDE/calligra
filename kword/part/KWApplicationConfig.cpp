/* This file is part of the KDE project
 * Copyright (C) 2007-2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWApplicationConfig.h"
#include "KWDocument.h"

#include <KoUnit.h>
#include <KoGlobal.h>
#include <KConfigGroup>

KWApplicationConfig::KWApplicationConfig()
        : m_viewFrameBorders(true),
        m_viewRulers(false),
        m_viewFormattingChars(false),
        m_viewFormattingBreak(false),
        m_viewFormattingSpace(false),
        m_viewFormattingEndParag(false),
        m_viewFormattingTabs(false),
        m_createBackupFile(true),
        m_statusBarShowPage(true),
        m_statusBarShowModified(false),
        m_statusBarShowMouse(false),
        m_statusBarShowZoom(true),
        m_zoom(100),
        m_zoomMode(KoZoomMode::ZOOM_WIDTH),
        m_autoSaveSeconds(KoDocument::defaultAutoSave()),
        m_defaultColumnSpacing(MM_TO_POINT(6))
{
}

void KWApplicationConfig::load(KWDocument *document)
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup interface = config->group("Interface");
//    setCursorInProtectedArea(interface.readEntry("cursorInProtectArea", true));
    // Config-file value in mm, default 10 pt
    m_viewRulers = interface.readEntry("Rulers", m_viewRulers);
    m_autoSaveSeconds = interface.readEntry("AutoSave", qRound(m_autoSaveSeconds / 60.0)) * 60; // read key in minutes
    m_createBackupFile = interface.readEntry("BackupFile", m_createBackupFile);

//    setNbPagePerRow(interface.readEntry("nbPagePerRow",4));
//    m_maxRecentFiles = interface.readEntry("NbRecentFile", 10);

    m_viewFormattingChars = interface.readEntry("ViewFormattingChars", m_viewFormattingChars);
    m_viewFormattingBreak = interface.readEntry("ViewFormattingBreaks", m_viewFormattingBreak);
    m_viewFormattingSpace = interface.readEntry("ViewFormattingSpace", m_viewFormattingSpace);
    m_viewFormattingEndParag = interface.readEntry("ViewFormattingEndParag", m_viewFormattingEndParag);
    m_viewFormattingTabs = interface.readEntry("ViewFormattingTabs", m_viewFormattingTabs);

    m_viewFrameBorders = interface.readEntry("ViewFrameBorders", m_viewFrameBorders);

    m_zoom = interface.readEntry("Zoom", m_zoom);
    m_zoomMode = static_cast<KoZoomMode::Mode>(interface.readEntry("ZoomMode", (int) m_zoomMode));

    m_statusBarShowPage = interface.readEntry("StatusBarShowPage", m_statusBarShowPage);
    m_statusBarShowModified = interface.readEntry("StatusBarShowModified", m_statusBarShowModified);
    m_statusBarShowMouse = interface.readEntry("StatusBarShowMouse", m_statusBarShowMouse);
    m_statusBarShowZoom = interface.readEntry("StatusBarShowZoom", m_statusBarShowZoom);

//    m_bShowDocStruct = interface.readEntry("showDocStruct", true);
//    m_viewModeType = interface.readEntry("viewmode", "ModeNormal");
//    setShowStatusBarShow(interface.readEntry("ShowStatusBarShow" , true));
//    setAllowAutoFormat(interface.readEntry("AllowAutoFormat" , true));
//    setShowScrollBar(interface.readEntry("ShowScrollBar", true));
//    if (isEmbedded())
//        m_bShowDocStruct = false; // off by default for embedded docs, but still toggleable
//    m_pgUpDownMovesCaret = interface.readEntry("PgUpDownMovesCaret", true);
//    m_bInsertDirectCursor= interface.readEntry("InsertDirectCursor", false);
//    m_globalLanguage=interface.readEntry("language", KGlobal::locale()->language());
//    m_bGlobalHyphenation=interface.readEntry("hyphenation", false);

//    setShowGrid(interface.readEntry("ShowGrid" , false));
//    setSnapToGrid(interface.readEntry("SnapToGrid", false));

    int undo = 30;
    KConfigGroup misc = config->group("Misc");
    if (misc.exists()) {
        undo = misc.readEntry("UndoRedo", -1);

        //load default unit setting - this is only used for new files (from templates) or empty files
        if (document && misc.hasKey("Units"))
            document->setUnit(KoUnit::unit(misc.readEntry("Units")));
        m_defaultColumnSpacing = misc.readEntry("ColumnSpacing", m_defaultColumnSpacing);
    }

//    if(undo!=-1)
//        setUndoRedoLimit(undo);

    //text mode view is not a good default for a readonly document...
//    if (!isReadWrite() && m_viewModeType =="ModeText")
//        m_viewModeType= "ModeNormal";

//    m_layoutViewMode = KWViewMode::create(m_viewModeType, this, 0 /*no canvas*/);

    KConfigGroup path = config->group("Kword Path");
    if (path.exists()) {
//        if (path.hasKey("expression path"))
//            m_personalExpressionPath = path.readPathEntry("expression path", QStringList());
        if (document)
            document->setBackupPath(path.readPathEntry("backup path", QString()));
    }

    // Load personal dict
    KConfigGroup spelling = KoGlobal::kofficeConfig()->group("Spelling");
//    m_spellCheckPersonalDict = spelling.readListEntry("PersonalDict");

}

void KWApplicationConfig::save()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ViewFormattingChars", m_viewFormattingChars);
    interface.writeEntry("ViewFormattingBreaks", m_viewFormattingBreak);
    interface.writeEntry("ViewFormattingEndParag", m_viewFormattingEndParag);
    interface.writeEntry("ViewFormattingTabs", m_viewFormattingTabs);
    interface.writeEntry("ViewFormattingSpace", m_viewFormattingSpace);
    interface.writeEntry("ViewFrameBorders", m_viewFrameBorders);
    interface.writeEntry("Zoom", m_zoom);
    interface.writeEntry("ZoomMode", (int)m_zoomMode);
//    interface.writeEntry("showDocStruct", m_bShowDocStruct);
    interface.writeEntry("Rulers", m_viewRulers);
//    interface.writeEntry("viewmode", m_viewModeType) ;
//    interface.writeEntry("AllowAutoFormat", m_bAllowAutoFormat);
//    interface.writeEntry("ShowGrid" , m_bShowGrid);
//    interface.writeEntry("SnapToGrid" , m_bSnapToGrid);
    interface.writeEntry("StatusBarShowPage", m_statusBarShowPage);
    interface.writeEntry("StatusBarShowModified", m_statusBarShowModified);
    interface.writeEntry("StatusBarShowMouse", m_statusBarShowMouse);
    interface.writeEntry("StatusBarShowZoom", m_statusBarShowZoom);
    interface.sync();
}

void KWApplicationConfig::setUnit(const KoUnit &unit)
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup misc = config->group("Misc");
    misc.writeEntry("Units", KoUnit::unitName(unit));
    misc.sync();
}
