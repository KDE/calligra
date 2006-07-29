Changes after KWord-1.4
=======================
- OASIS OpenDocument support:
   Fixed loading of headers and footers (#107424)
   Implemented saving of headers and footers
   Fix for text:h without outline-level
   Simplified the XML by removing spurious <span> tags due to misspelled words
   Save office:font-face-decls tag so that OOo loads the fonts
   Fixed loading of font sizes specified as a percentage
   Implemented loading of list indentation (space-before) (#109223)
   Improved OOo compatibility when saving page breaks
   Save first page number the right way so that OOo can load it
- Text layouting: fix for formatting of multi-frame paragraphs (#65059)
- Overwrite mode (activated with the "Insert" key)

Changes after KWord-1.4-beta1
=============================
- Fixed crash when editing a cell on loading (#104982).
- Many footnotes bugs have been solved (#85148,, #72506, #57298, #62360, #84678).
- Show the content of the footnote/endnote in the status barwhen the mouse cursor is placed over a footnote/endnote reference (part of feature #48794).
- Added an apply button to the frame property and the resize column dialog.
- Added a reset button to the resize column dialog.
- Resize column dialog: the maximum width for a cell is the width of the table's parent frame if the table is floating.
- All KDoubleNumInputs have been replaced KoUnitDoubleSpinBoxes.
- Disable the formula toolbar from start if no formula frame is activated.

Changes between KWord-1.3.x and KWord-1.4-beta1
===============================================
See ../changes-1.4-beta1

Changes between KWord 1.3.2 and 1.3.3
=====================================
- Fix the 3 style menus for any translation (#61007)
- OASIS: proper loading/saving of inline tables
- Disable creation of non-inline tables
- OASIS: default tab width

Changes after KWord-1.3
=======================
- Reorganized and simplified GUI of the configuration dialog
- Support for "protect content" in all types of framesets
- Support for different run-around-gaps on every side of the frame
- Use kspell2
- Copy/paste between kword/kpresenter
- Add statistic variable

Changes between KWord-1.3-rc and KWord-1.3
==========================================
- Fixed swapped resize mouse cursors with Qt-3.1.2
- Fixed table column resizing (avoid overlapping columns)

Changes after KWord 1.3 rc (1.2.94)
===================================
- Fixed crash in table operations (#66349)
- Fixed crash in table iterator
- Fixed "edit frameset" for tables in doc structure panel (#67084)
- Save "no underline" explicitely if the paraglayout says "underline" but the
  user removed the underline (#67735)
- Fixed successive image DnD
- Fixed the default document font (#69166)

Changes after KWord 1.3 beta4
=============================
- fix for frames extending beyond the page getting the wrong size in 'preview mode'
- Fixed wrong layouting of inline frames (e.g. pictures)
- Don't select inline frame after pasting text containing it (only select non-inline frames)
- Improved "create style from selection": use format at cursor if no selection,
  allow to type the name of an existing style to update it, and select the style
  for the current paragraph after creating it.

Changes after KWord-1.3-beta3
=============================
- Fixed handling of part frames (clicking didn't select it anymore, #63041)
- Fixed cursor shape when pressing Ctrl.
- Unbreak text view mode (cursor disappeared, #57554)

Changes after KWord-1.3-beta2
=============================
- Right-click on a selected frame shouldn't unselect it
- Esc to deselect all frames
- Fixed line-height bug due to hyphenation
- Make the text view mode work again
- Fix "frame borders always painted/printed even when off" (#45907)

Changes after KWord-1.3-beta1
=============================

- Frames default to "auto-resize" mode instead of "auto-create-new-page"
- Better handing of "auto-resize" mode (manually-set size acts as minimum size)
- Make the find dialog avoid the area where the match is (requires kdelibs-CVS, will be in kde-3.2)
- New: PDF import filter
(see also koffice/lib/CHANGES, especially the kotext part)

Changes after KWord-1.2
=======================
- Spellchecker can spell check only a selection
- Shadow is a character property
- Language added as a new character property
- Paragraph numbering: simple lists are not restarted by simple text in between
- Paragraph numbering: new properties "restart numbering at this paragraph",
  "number of parent levels to display".
- Add "copy link"
- Add "Add to bookmark"
- Now we can change "picture path" / "personal expression path"
- Add "Remove link"
- Add font properties : change relative font size when we sub/super script font
- Add support to add offset from baseline
- Now koinsertlink use "Recent document".
- Add support to insert directly cursor
- Add support for "word by word" format attribute.
- Add new variable : Next page number, previous page number
- Add time/date variable offset
- Fix #24242 add variable : created/modified/printed document date
- Now we can copy text of comment.
- Add support for convert table to text
- Add support for font attribute ( lowercase/uppercase )
- Add double click on frame => open frame dialogbox
- Add autocorrection : "Capitalize name of days"
- New font attribute : small caps
- The list of words to be ignored by the spellchecker can be edited
- Add support for convert text to textbox
- Now we can add/remove word in personal dictionary
- The path where backup files are saved is now configurable
- Improve statistics dialogbox : count number of lines.
- The user can more easily add personal expressions to be inserted
- New types of line spacing: "At least"/"Multiple"
- Add horizontal line
- Text can be sorted
- Use new kospell API
- Now right button gives spell check result
- Load pictures only one time when inserting from file (Good for remote files)
- Hyphenation support. Configurable as a character property (to disable hyphenation on specific words).
- New checkbox in the style manager: "Include in table of contents".


Changes between KWord-1.2-RC1 and KWord-1.2
===========================================
Bugfixes:
- Fix update ruler tabs when we switch between text object
- Fix move up/down table/frame style
- Fix restore shortcut when we remove/add/rename style
- Fix restore shortcut when we remove/move/add/rename frame/table style
- Fix update resize handle when we change cell border
- Fix update resize handle when we change frame/table style.

KoText library fixes:
- Fix move up/down style
- Fix crash when we create a new style and delete this style.

Layout:
- Fixed layout of paragraph numbering in Right-To-Left mode.
- First line indent is now from the right in Right-To-Left mode.

Crashes:
- Fixed crash when removing pages (and the page number is shown in header/footer)


Changes between KWord-1.2-beta2 and KWord-1.2-RC1
===================================================
New:
- Like all of KOffice: a readonly GUI for embedding into Konqueror
- More DCOP calls

Layout/painting fixes:
- Fixed WYSIWYG algorithm, no more negative spaces at some zoom levels.
- Transparency painting fixes: transparent frames, including text frames.
Note that transparent images do not print properly with the current Qt, Qt 3.1 will fix it.
- Footnote layout fix, when multiple footnotes are on the same page and the last one won't fit
- Improved algorithm for the way the text flows between frames of the same frameset.
- Zigzag lines for background checked words
- Fixed paragraph height increasing when inserting a footnote.
- Fixed the white lines during the painting of inline frames
- Fixed footer layout in some first/even/odd footer settings
- Fixed baseline alignment for variables
- Delete overflowing frame (in auto-resize mode) when removing text from it
- Compress repaint events coming from the background spell-checking

Bugfixes:
- Several fixes for the readonly case
- Unified picture handling (images and cliparts)
- Improvements to frame dialog when opened for multiple frames, many crashes fixed.
- Fixed frame-creation rectangle remaining visible in margin after right click.
- Many fixes for the background spellchecking
- Many fixes for "Insert File" (pictures, embedded objects, inline frames...)
- Ability to use the text view mode for any text frameset
- Embedded document must be selected before clicking on them starts editing them.
- More KOffice-1.1 compatibility on saving (when explicitely requested)
- Save correct variable value even when variable codes are displayed.
- When making an inline frame non-inline, ensure it appears on top (bigger z-order)
- 'Copy' exports paragraph numbers with the text.
- Keep key shortcuts when rebuilding dynamic menus (e.g. style lists)
- Fixed 'raise frame' and 'lower frame' (handle equality case)
- Several GUI fixes
- No header/footer configuration in DTP mode
- No page layout, frame resizing etc. in the text view mode.
- More robustness in DCOP calls (to prevent crashes)
- Faster pasting of text
- Fix use keyboard when frameset is protect content.
- Fix reload PgUp and PgDn config
- Fixed disappearing frame after undoing deletion of inline frame
- Inserting a non-inline table makes the text flow around it by default

KoText library fixes:
- Fixed preview in paragraph dialog (was truncated)
- Fixed font size applied by "Find/Replace with formatting" and "Autocorrection with formatting".
- Fixed background color not applied by "Find/Replace with formatting".
- Fixed crash when we remove "autocorrect word formatted"
- Fixed mem leak in autocorrect dialogbox (autocorrect list was not deleted )
- Fixed non/working undo/redo when changing paragraph settings for a whole text frameset
- Fixed calculation of centering (visible on very short paragraphs)
- The height of the cursor now depends on the character on the left of the cursor
(instead of being as high as the biggest char in the paragraph).

Crashes:
- Do not crash when pasting an embedded object (not implemented though)
- Fixed crash when removing a footnote variable
- Fixed crash when we close kword and "special char dialogbox" is not closed
- Fixed several problems with footnote layout
- Fixed crash when we delete style that is also used as a 'following style'.
- Fixed crash with bookmarks
- Fixed several memory leaks
- Fixed crash when we canceled create time/date variable.


Changes between KWord-1.2-beta1 and KWord-1.2-beta2
===================================================
- Footnotes
- "Apply autoformat" feature
- Extended Find/Replace to allow specifying text formatting
- Support for frame margins
- Support for frame "runaround gap"
- Choice of which side of the frame the text should run around (left, right, biggest)
- Creating a style directly from the selection
- Paragraph selection, clicking on the left of the text frame
- "Protect content" feature
- Double line border type for frames and paragraphs
- New dialogbox to resize a column in a table
- Add new font format (improve underline, strikeout )
- Add support to protect frameset size and position.
- Fix the loading, displaying and saving of old images (KOffice 1.0 type)
- The default width used by tab stops is now configurable
- Headers and footers can be both "different on first page" and "different
  on odd/even pages" at the same time.
- Ability to save an image from a document to the disk
- Cursor position is saved into the file, to restore it when reopening the document
- The "text" view mode is finally operational.
- Add option "Enable cursor in protected text object"
- Now we can configure which type of formatting char we want to display
- Now we can configure footnote line separator
- Add support for framestyle (borders and backgroundcolor)
- Creating a framestyle from a selected frame
- Add support for tablestyles (combination of framestyle and style)
- Add templates for tables (built up from tablestyles)
- Add support to insert Native file 
- Import frame/table style from file
- Import text style from file
- Add display field code ( for variable)
- We can change autoformat language
- Painting of transparent images
- Fixed handling of embedded documents


Changes between KWord-1.1.1 and KWord-1.2-beta1
===============================================

Text layout and painting redesigned around WYSIWYG concept.
KWord is also BiDi-enabled.

Soft hyphens (inserted with Ctrl+'-')
Non-breakable-spaces (inserted with Ctrl+' ' or menuitem)
Support for hyperlinks, for text background color (marker).
Improved handling of tabulators in ruler.
Auto-hide mouse cursor while typing.

"Change picture" feature.
SVG support (for cliparts).
Module for SVG previews in Konqueror.

Rewrite of tables where positioning is much more fool-proof and table-borders actually work.
More intelligent table-cell-scaling for cells with mixed text lengths

Text shadow feature.
Double-underline feature.
Ability to choose where to inline a frame.
Setting tabs via the rulers is more intuitive.
Add link (internet link/ address mail / file link)
Add dcop interface : now we can insert text, change text format, etc...
Frame z-order ("Raise frame", "Lower frame")
Preliminary support for background spell checking
Support for manual completion (use CTRL+E)
Support for note
Ability to associate key shortcuts to styles
"Section Title" variable

Triple click selects a paragraph.
New "PageUp/PageDown moves the caret" option, which defaults to false.
 The new behaviour for PageUp/PageDown is to move the scrollbars, not the caret.


Changes between KWord-1.1 and KWord-1.1.1
=========================================
Fixed crash when undoing "apply a style" (QTextFormatCommand, #34556)
Fixed printing from preview mode
Fixed default value for runaround when loading a frame.
Save all frames of the main frameset even in WP mode (to keep borders etc.)
Fixed list of allowed image formats in image selector dialog
Fixed crash with Ctrl+K on empty paragraph
Fixed consecutive tab problems
Workaround for slow clipboard format checking in Qt2 
Hack around Tru64-compiler bug (KWTextFormat::~KWTextFormat())


Changes between KWord-1.1-beta3 and KWord-1.1-rc1
=================================================

Improved auto-scrolling.
[...]


Changes between KWord-1.1-beta2 and KWord-1.1-beta3
===================================================

New view mode: preview (multiple pages per row)
Inline items (variables, images, frames) are now correctly drawn when selected,
can be made non-inline and back.
Improved readonly mode, when embedding into konqueror (can now select and copy).
Insert Frame/Page Break feature added (also as part of styles).
Apply button added to the stylist.
Zoom value can be set arbitrarily by the user (editable combobox).
Auto-correction, typographical quotes.
Formatting Characters feature.
"1.5 lines" and "double" linespacing (also as part of styles).
Insertion of predefined expressions (configurable).
Auto-switching between editing of text and editing of frames.
All types of frames can be set as "copy".
Flesch reading ease score and enhanced layout added to the statistics dialog.
Insert clipart (WMF).
More spellchecking options (needs kdelibs-2.2-cvs).
Added the ability for frames to grow and shrink when more or less text flows in them.
Added undo-redo availability to many user-actions.
New bullets-numbering dialog (visible in the stylist and the paragraph editor)
Enhanced context-menus (Right mouse buttons popup)
Added the possibility to change the case (capitals) of a selection of text.

Useability improvements (better mouse selection, a few new keybindings).
Custom variables appear in a submenu.
Major speedups for long documents (paragraph formatting, loading),
and painting fixes.
Fixed recalculation of "page number" variable in header or footer.
Painting and run-around-frame fixes (e.g. in multi-columns mode).
Fixes for the creation and removal of pages.
Fixed crash when embedding into Konqueror.
Fixed Table Of Contents creation and updates.
Fixed placement and drawing of tables.
Many other fixes, this list is far from complete.


Changes between KWord-1.1-beta1 and KWord-1.1-beta2
===================================================

Copying frames has been implemented, fixing headers and footers.
Implemented tabulators (left/right/center/decimal aligned).
Implemented floating framesets, mainly for inline formulas and tables.
KWord can now be embedded into another KOffice application.
More precise coordinate system (especially for frames).
Many improvements for tables (including splitting and merging cells).
Highlight mispelled word during spell-checking, and spell-check all framesets.
UI improvements, undo/redo fixes, font-size fix, border-drawing improved, etc.
Many new variables available.
Redesigned style editor (stylist), now all in one dialog.
When changing a style, only the changes are applied to existing paragraphs (not the whole style).
Fixed drawing and printing of embedded objects.
Respect user's color scheme.
Improved visual appearance (page shape, with shadow).


Main changes between KWord-0.8 (KOffice-1.0) and KWord-1.1-beta1
========================================================================

New overall design and new formatting engine.

Better zooming; adapts to the DPI settings (of the screen and of the printer)
This means, if your X server is correctly configured, the page on screen has
the same size as the real paper page.

Much better performance with huge documents (the text is formatted
 as a background job, the repainting is done much better etc.).

File should be smaller than before (smarter way of saving
 the formatting information)

Real document/view separation, working with different views on the same
document is fully functional, cursors are in each view.

Brand new formula widget (with doc/view design, zoom support etc.).
Redesigned table support.
Much improved find/replace functionality.

Other new features:
 - Real bullets instead of using characters as bullets.
 - Line-level breaking of paragraphs at end of frame/page
 - Right margin for a paragraph.
 - Undo/redo fully working (all text operations, but also frame operations etc.)
 
Fixes:
 - Embedding a kpresenter part into kword now works
 - Copy/Paste and Drag-and-drop work as expected (instead of for full paragraphs only)

When Qt3 is out, KWord will support bi-directional editing of text (for hebrew, arabic etc.).
