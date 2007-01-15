#!/usr/bin/env python

# Import the KWord module.
import KWord

# Get the first frameset which contains the main textdocument.
fs = KWord.frameSet(0)
if fs == None:
    raise "No first frameset"

# Get the main textdocument.
doc = fs.textDocument()
if doc == None:
    raise "No textdocument within the first frameset"

# Set the default cascading stylesheet.
doc.setDefaultStyleSheet(
    (
        "h1 { color:#0000ff; }"
        "h2 { color:#0000aa; }"
    )
)

# Set the content of the main textdocument. Previous content
# got replaced with the new HTML content.
doc.setHtml(
    (
        "<h1>Python Sample: Lists with Cursor</h1>"
        "<p align=\"justify\">This python sample script demonstrates how to use the "
        "KWord cursor-interface to create lists with content and how to format and "
        "style such lists."
        "<p></p>" # seems to be needed else the lastCursorPosition() is within the dl-block :-(
    )
)

# Get a cursor located at the end of the main textdocument.
cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.firstCursor()
#cursor = doc.lastCursor()

def addList(text,liststyle):
    global KWord, cursor
    # The block seems to be needed cause else additional insertHtml-calls don't respect the line-break whyever.
    cursor.insertDefaultBlock()
    #cursor.insertBlock()
    # We like to create a new list
    cursor.insertHtml("<br><h2>%s</h2>" % text)
    # Create a new style and set the liststyle
    s = KWord.addParagraphStyle("My%sStyle" % text)
    s.setListStyle( getattr(s,liststyle) ) # e.g. s.setListStyle( s.SquareItem )
    # Create the list and apply the style
    l = cursor.insertList()
    l.setStyle(s)
    # Fill the list with some items
    for i in range(1, 6):
        if i != 1:
            cursor.insertBlock()
        cursor.insertHtml( "item nr=%i count=%i" % (i,l.countItems()) )

addList("Square","SquareItem")
addList("Disc","DiscItem")
addList("Circle","CircleItem")
addList("Decimal","DecimalItem")
addList("AlphaLower","AlphaLowerItem")
addList("AlphaUpper","AlphaUpperItem")
addList("RomanLower","RomanLowerItem")
addList("RomanUpper","RomanUpperItem")

#TODO
#prefix+suffix
#restart numbering
#startvalue
#aligned
#counted
