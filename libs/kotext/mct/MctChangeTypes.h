/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#ifndef MCTCHANGETYPES_H
#define MCTCHANGETYPES_H

enum MctChangeTypes : unsigned int {

    AddedString = 0,                        // Added string
    AddedStringInTable = 1,                 // Added string in table
    RemovedString = 2,                      // Removed string
    RemovedStringInTable = 3,               // Removed string in table
    MovedString = 4,                        // Moved string
    MovedStringInTable = 5,                 // Moved string in table
    ParagraphBreak = 6,                     // Add paragraph break
    ParagraphBreakInTable = 7,              // Add paragraph break
    DelParagraphBreak = 8,                  // Delete paragraph break
    DelParagraphBreakInTable = 9,           // Delete paragraph break
    StyleChange = 10,                       // Style change
    StyleChangeInTable = 11,                // Style change in table
    AddedTextFrame = 12,                    // Added TextFrame
    AddedTextFrameInTable = 13,             // Added TextFrame in table
    RemovedTextFrame = 14,                  // Removed TextFrame
    RemovedTextFrameInTable = 15,           // Removed TextFrame in table
    AddedTextGraphicObject = 16,            // Added TextGraphicObject
    AddedTextGraphicObjectInTable = 17,     // Added TextGraphicObject in table
    RemovedTextGraphicObject = 18,          // Removed TextGraphicObject
    RemovedTextGraphicObjectInTable = 19,   // Removed TextGraphicObject in table
    AddedEmbeddedObject = 20,               // Added EmbeddedObject
    AddedEmbeddedObjectInTable = 21,        // Added EmbeddedObject in table
    RemovedEmbeddedObject = 22,             // Removed EmbeddedObject
    RemovedEmbeddedObjectInTable = 23,      // Removed EmbeddedObject in table
    AddedTextTable = 24,                    // Added TextTable
    AddedTextTableInTable = 25,             // Added TextTable in table
    RemovedTextTable = 26,                  // Removed TextTable
    RemovedTextTableInTable = 27,           // Removed TextTable in table
    AddedRowInTable = 28,                   // Added Row in table
    RemovedRowInTable = 29,                 // Removed Row in table
    AddedColInTable = 30,                   // Added Col in table
    RemovedColInTable = 31                  // Removed Col in table

};


#endif // MCTCHANGETYPES_H
