/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <QString>


//  For accelerator sliding and scrolling

/*!
* Cordinates for horizontalscrolling
*/

#define XCORDINATELIMITLEVEL1    70
#define XCORDINATELIMITLEVEL2   100
#define XCORDINATELIMITLEVEL3   200
#define XCORDINATELIMITLEVEL4   300

/*!
* Cordinates for virticalscrolling
*/

#define YCORDINATELIMITLEVEL1    70
#define YCORDINATELIMITLEVEL2   150
#define YCORDINATELIMITLEVEL3   200
#define YCORDINATELIMITLEVEL4   300

/*!
* Levels of the vibration
*/
#define VIBRATIONLEVEL1  "PatternAcceleratora"
#define VIBRATIONLEVEL2  "PatternAcceleratorb"
#define VIBRATIONLEVEL3  "PatternAcceleratorc"
#define VIBRATIONLEVEL4  "PatternAcceleratord"
#define VIBRATIONLEVEL5  "PatternAcceleratore"
#define VIBRATIONLEVEL6  "PatternAcceleratorf"
#define VIBRATIONLEVEL7  "PatternAcceleratorg"
#define VIBRATIONLEVEL8  "PatternAcceleratorh"
#define VIBRATIONLEVEL9  "PatternAcceleratori"
#define VIBRATIONLEVEL10  "PatternAcceleratorj"

/*!
* Range limits for  sliding
*/
#define LIMITFORENTERINGRANGE   700
#define LIMITFOREXITINGRANGE    400

//////////////////////////////////////////////

/*!
* Size of full screen button
*/
#define FS_BUTTON_SIZE 64

/*!
* Path for full screen icon
*/
const QString FS_BUTTON_PATH(":/images/48x48/general_fullsize.png");

/*!
* Path for presentation back icon in full screen mode
*/
const QString FS_PPT_BACK_BUTTON_PATH(":/images/48x48/general_back.png");

/*!
* Path for presentation forward icon in full screen mode
*/
const QString FS_PPT_FORWARD_BUTTON_PATH(":/images/48x48/general_forward.png");

/*!
* Path for splash screen Calligra logo
*/
const QString SPLASH_LOGO_CALLIGRA_PATH(":/images/178x200/calligramobile.png");
/*!
 * * Calligra pixmap in about dialog
 */
const QString ABOUT_DIALOG_CALLIGRA_PIXMAP("/usr/share/icons/hicolor/64x64/apps/calligramobile.png");
/*!
 * * Nokia pixmap in about dialog
 * */
const QString ABOUT_DIALOG_NOKIA_PIXMAP("/usr/share/icons/hicolor/scalable/hildon/about_nokia_logo.png");
/*!
 * * notes view icon
 * */
const QString VIEW_NOTES_PIXMAP(":/images/64x64/PresentationDrawTool/viewnotes.png");
/*!
* Path New Document Template
*/
const QString NEW_WORDDOC("/usr/share/calligramobile-templates/.templates/NewDocument.odt");
/*!
* Path New Presentation Template
*/
const QString NEW_PRESENTER ("/usr/share/calligramobile-templates/.presentationtemplates/");
/*!
* Path New Spreadsheet Template
*/
const QString NEW_SPREADSHEET("/usr/share/calligramobile-templates/.templates/NewSpread.ods");

/*!
* Style for full screen button
*/
const QString FS_BUTTON_STYLE_SHEET("background-color: black; \
                                     border: none; \
                                     border-radius: 5px;");
/*!
 * DBus FreOffice Path
 */
const QString FREOFFICE_PATH("/com/nokia/CalligraMobile");

/*!
 * DBus FreOffice ServiceName
 */
const QString FREOFFICE_SERVICE_NAME("com.nokia.CalligraMobile");

#endif // COMMON_H
