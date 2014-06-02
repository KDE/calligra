/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
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

#ifndef KRITASTEAMACHIEVEMENTS_H
#define KRITASTEAMACHIEVEMENTS_H

// List of  IDs for the achievements in Steam

// Triggered events

/* STEAM_ACHIEVEMENT_SUBMITTEDBUG
 * "bug_submitted"
 * Achievement: "Just Another Bug Hunt"
 * Awarded when the user submits a bug report.
 */
#define STEAM_ACHIEVEMENT_SUBMITTEDBUG "bug_submitted"

/* STEAM_ACHIEVEMENT_EASTEREGG
 * "easter_egg"
 * Achievement: "Easter Egg Hunt"
 * Awarded when the user performs the following actions in sequence:
 * 1. Creates a new image at 1920x1080 resolution
 * 2. Use the Circle tool to draw a circle on the canvas
 * 3. Save the file as KRA with the name "FoundTheEgg"
 *
 * Congratulations!!!
 * If you're reading this, this achievement was for you ;)
 */
#define STEAM_ACHIEVEMENT_EASTEREGG "easter_egg"



// ==============================================
// Infrastructure events
// (Require testing against external server)
// ==============================================

/* STEAM_ACHIEVEMENT_HACKER
 * "hacker"
 * Achievement: "Open-Source Hacker"
 * Awarded to users who submit a patch to the krita source.
 * User will need to notify of Steam ID in order to mark this
 * as being awarded on the achievement server.
 */
#define STEAM_ACHIEVEMENT_HACKER "hacker"


#endif //KRITASTEAMACHIEVEMENTS_H
