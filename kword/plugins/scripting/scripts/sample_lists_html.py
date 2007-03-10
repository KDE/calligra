#!/usr/bin/env python

# Import the KWord module.
import KWord

# Get the TextDocument.
doc = KWord.mainFrameSet().document()

# Set the default cascading stylesheet.
doc.setDefaultStyleSheet(
    (
        # Style for the headers.
        "h1 { color:#0000ff; }"
        "h2 { color:#000099; }"

        # Style for the unsorted list. This does not seem to work :-(
        #".ulist ul { list-style-type:disc; }"
        #".ulist ul { list-style-type:none; }"
        #".ulist ul { list-style-type:circle; }"
        #".ulist ul { list-style-type:square; }"

        # Style for the items in the unsorted list.
        ".ulist1 li { margin-left:1em; }"
        ".ulist2 li { margin-left:1em; color:#aa0000; }"
        ".ulist3 li { margin-left:1em; color:#00aa00; }"
        ".ulist4 li { margin-left:1em; color:#006666; }"

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
        ".olist1 li { margin-left:1em; }"
        ".olist2 li { margin-left:1em; color:#aa0000; }"
        ".olist3 li { margin-left:1em; color:#00aa00; }"
        ".olist4 li { margin-left:1em; color:#006666; }"
    )
)

# Set the content of the main textdocument. Previous content
# got replaced with the new HTML content.
doc.setHtml(
    (
        "<h1>Python Sample: Lists with HTML</h1>"
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
        "<h2>Unsorted list (UL)</h2>"
        "<ul class=\"ulist1\">"
            "<li>First main item"
                "<ul class=\"ulist2\">"
                    "<li>First child item of the first main item</li>"
                    "<li>Second child item of the first main item</li>"
                "</ul>"
            "</li>"
            "<li>Second main item"
                "<ul class=\"ulist3\">"
                    "<li>First child item of the second main item"
                        "<ul class=\"ulist4\">"
                            "<li>First sub-child item of the first child item which is child of the second main item. Propably somewhat confusing but it is used as example only anyway to fill this item with some more text.</li>"
                            "<li align=\"right\">Second sub-child item aligned right</li>"
                            "<li align=\"justify\">Theird sub-child item which has the align set to \"justify\" and should just like the intro-sentence be, well, aligned justified. Sounds at least somewhat logical in some situations with some conditions sometimes.</li>"
                        "</ul>"
                    "</li>"
                    "<li>Second child item of the second main item</li>"
                    "<li>Theird child item of the second main item</li>"
                "</ul>"
            "</li>"
        "</ul>"
        "<h2>Sorted list (OL)</h2>"
        "<ol class=\"olist1\">"
            "<li>First main item"
                "<ol class=\"olist2\">"
                    "<li>First child item of the first main item</li>"
                    "<li>Second child item of the first main item</li>"
                "</ol>"
            "</li>"
            "<li>Second main item"
                "<ol class=\"olist3\">"
                    "<li>First child item of the second main item"
                        "<ol class=\"olist4\">"
                            "<li>First sub-child item of the first child item which is child of the second main item. Propably somewhat confusing but it is used as example only anyway to fill this item with some more text.</li>"
                            "<li>Second sub-child item</li>"
                        "</ol>"
                    "</li>"
                    "<li>Second child item of the second main item</li>"
                    "<li>Theird child item of the second main item</li>"
                "</ol>"
            "</li>"
        "</ol>"
        "<h2>List of definitions (DT)</h2>"
        "<dl>"
            "<dt>KDE</dt>"
                "<dd>The Desktop Environment</dd>"
            "<dt>KOffice</dt>"
                "<dd>The Integrated Office Suite</dd>"
            "<dt>KWord</dt>"
                "<dd>The word-processing and desktop publishing application</dd>"
            "<dt>Kross</dt>"
                "<dd>The Scripting Framework</dd>"
            "<dt>Kaugummi</dt>"
                "<dd>The german word for chewing gum and useful if you like to stop smoking :-)</dd>"
        "</dl>"
    )
)
