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
        # Style for the headers.
        "h1 { color:#0000ff; }"
        "h2 { color:#0000aa; }"
        "h3 { color:#000066; }"

        # Style for the unsorted list. This does not seem to work :-(
        #".ulist ul { list-style-type:disc; }"
        #".ulist ul { list-style-type:none; }"
        #".ulist ul { list-style-type:circle; }"
        #".ulist ul { list-style-type:square; }"

        # Style for the items in the unsorted list.
        ".ulist li { margin-left:1em; }"

        # Style for the sorted list. This does not seem to work :-(
        #".olist ul { list-style-type:decimal; }"
        #".olist ul { list-style-type:none; }"
        #".olist ul { list-style-type:lower-roman; }"
        #".olist ul { list-style-type:upper-roman; }"
        #".olist ul { list-style-type:decimal-leading-zero; }"
        #".olist ul { list-style-type:lower-greek; }"
        #".olist ul { list-style-type:lower-latin; }"
        #".olist ul { list-style-type:upper-latin; }"
        #".olist ul { list-style-type:armenian; }"
        #".olist ul { list-style-type:georgian; }"

        # Style for the items in the sorted list.
        ".olist li { margin-left:1em; }"
    )
)

# Set the content of the main textdocument. Previous content
# got replaced with the new HTML content.
doc.setHtml(
    (
        "<h1>Python Sample: Lists</h1>"
        "<p align=\"justify\">This script demonstrates how to work in KWord with lists using the Python "
        "scripting language. The whole content displayed here was created direct within "
        "the Python script. You are also able to use Ruby or JavaScript. All supported "
        "scripting languages are accessing the same exposed functionality to flexible "
        "manipulate KWord documents.<br>"
        "We are using stylesheets to color the headers, align text and indent the items. You "
        "are able to use the whole functionality of CSS and HTML to create or format "
        "content or just use the cursor-functionality to deal with the content on a "
        "more abstract level.</p>"
        "<p align=\"center\"><b>KOffice 2.0 to maximize your productivity</b></p>"
        "<p align=\"right\"><a href=\"http://www.koffice.org\">http://www.koffice.org</a></p>"
        "<h2>Some lists created with HTML</h2>"
        "<h3>Unsorted list (UL)</h3>"
        "<ul class=\"ulist\">"
            "<li>First main item"
                "<ul>"
                    "<li>First child item of first main item</li>"
                    "<li>Second child item of first main item</li>"
                "</ul>"
            "</li>"
            "<li>Second main item"
                "<ul>"
                    "<li>First child item of second main item"
                        "<ul>"
                            "<li>First sub-child item of the first child item which is child of the second main item. Propably somewhat confusing but it is used as example only anyway to fill this item with some more text.</li>"
                            "<li>Second sub-child item</li>"
                        "</ul>"
                    "</li>"
                    "<li>Second child item of second main item</li>"
                "</ul>"
            "</li>"
        "</ul>"
        "<h3>Sorted list (OL)</h3>"
        "<ol class=\"olist\">"
            "<li>First main item"
                "<ol>"
                    "<li>First child item of first main item</li>"
                    "<li>Second child item of first main item</li>"
                "</ol>"
            "</li>"
            "<li>Second main item"
                "<ol>"
                    "<li>First child item of second main item"
                        "<ol>"
                            "<li>First sub-child item of the first child item which is child of the second main item. Propably somewhat confusing but it is used as example only anyway to fill this item with some more text.</li>"
                            "<li>Second sub-child item</li>"
                        "</ol>"
                    "</li>"
                    "<li>Second child item of second main item</li>"
                "</ol>"
            "</li>"
        "</ol>"
        "<h3>List of definitions (DT)</h3>"
        "<dl>"
            "<dt>KDE</dt>"
                "<dd>The Desktop Environment</dd>"
            "<dt>KOffice</dt>"
                "<dd>The Integrated Office Suite</dd>"
            "<dt>KWord</dt>"
                "<dd>The word-processing and desktop publishing application</dd>"
        "</dl>"
        "<p></p>" # seems to be needed else the lastCursorPosition() is within the dl-block :-(
    )
)

# Get a cursor located at the end of the main textdocument.
cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.firstCursor()
#cursor = doc.lastCursor()

# Insert a heading.
cursor.insertHtml("<h2>Some lists created with a Cursor</h2>")

# The block seems to be needed cause else additional insertHtml-calls don't respect the line-break whyever.
cursor.insertBlock()

# Demonstrate bullet lists.
cursor.insertHtml("<h3>Bullets</h3>")
l = cursor.insertList()
for i in range(1, 6):
    if i != 1:
        cursor.insertBlock()
    cursor.insertHtml( "item nr=%i count=%i" % (i,l.countItems()) )

#cursor.insertHtml("<h3>Bullets with prefix and suffix</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Lists</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Roman</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Restart Numbering</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Numbering with different startvalue</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Counted item</h3>")
#cursor.insertBlock()
#cursor.insertHtml("<h3>Right aligned</h3>")
#cursor.insertBlock()
