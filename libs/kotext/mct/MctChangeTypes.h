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
