#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import Words, time, sys, os

#if Words.pageCount() < 1: Words.insertPage(0)
#doc = Words.frameSet(0).document()

doc = Words.mainFrameSet().document()

# Set the default cascading stylesheet.
doc.setDefaultStyleSheet(
    (
        "li { margin-left:1em; }"
        ".my1 { text-transform: lowercase; }"
        ".my2 { text-transform: uppercase; }"
        ".my3 { text-transform: capitalize; }"
    )
)

# Set the html with some general informations.
doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Sample: Text</font></h1>"
        "<p><i>italic</i> and <b>bold</b> and <u>underlined</u> and a <a href=\"test\">link</a></p>."

        "<div class='my1'>lOwErCaSe</div>"
        "<div class='my2'>uPpErCaSe</div>"
        "<div class='my3'>capitalize</div>"

        "<ul>"
        "<li>Time: <b>%s</b></li>"
        "<li>Operating System: <b>%s</b></li>"
        "<li>Python Version: <b>%s</b></li>"

        "<li>Documents: <b>%s</b></li>"
        "<li>Views: <b>%s</b></li>"
        "<li>Windows: <b>%s</b></li>"

        "<li>PageCount: <b>%s</b></li>"
        "<li>FrameSet Count: <b>%s</b></li>"
        "<li>Frame Count: <b>%s</b></li>"

        "<li>Document Url: <b>%s</b></li>"

        "<li>Title: <b>%s</b></li>"
        "<li>Subject: <b>%s</b></li>"
        "<li>Keywords: <b>%s</b></li>"
        "<li>Abstract: <b>%s</b></li>"

        "<li>Author Name: <b>%s</b></li>"
        "<li>Author EMail: <b>%s</b></li>"
        "<li>Author Company: <b>%s</b></li>"
        "<li>Author Postion: <b>%s</b></li>"
        "<li>Author Telephone: <b>%s</b></li>"
        "<li>Author Telephone Work: <b>%s</b></li>"
        "<li>Author Telephone Home: <b>%s</b></li>"
        "<li>Author Fax: <b>%s</b></li>"
        "<li>Author Country: <b>%s</b></li>"
        "<li>Author Postal Code: <b>%s</b></li>"
        "<li>Author City: <b>%s</b></li>"
        "<li>Author Street: <b>%s</b></li>"
        "<li>Author Initial: <b>%s</b></li>"

        "</ul>"
        "<pre>Some text in a pre-tag</pre>"
        "<blockquote>Some text in a blockquote-tag</blockquote>"
        "<p>Some more text in a paragraph...</p>"
    ) % (
        time.strftime('%H:%M.%S'),
        ' '.join(os.uname()),
        sys.version,

        ','.join(Words.application().getDocuments()),
        ','.join(Words.application().getViews()),
        ','.join(Words.application().getWindows()),

        Words.pageCount(),
        Words.frameSetCount(),
        Words.frameCount(),

        Words.document().url(),

        Words.document().documentInfoTitle(),
        Words.document().documentInfoSubject(),
        Words.document().documentInfoKeywords(),
        Words.document().documentInfoAbstract(),

        Words.document().documentInfoAuthorName(),
        Words.document().documentInfoCompanyName(),
        Words.document().documentInfoAuthorPostion(),
        Words.document().documentInfoEmail(),
        Words.document().documentInfoTelephone(),
        Words.document().documentInfoTelephoneWork(),
        Words.document().documentInfoTelephoneHome(),
        Words.document().documentInfoFax(),
        Words.document().documentInfoCountry(),
        Words.document().documentInfoPostalCode(),
        Words.document().documentInfoCity(),
        Words.document().documentInfoStreet(),
        Words.document().documentInfoInitial(),
    )
)

# Add a list of details about the framesets and frames.
html = "<p><b>Framesets:</b></p><ul>"
for i in range( Words.frameSetCount() ):
    frameset = Words.frameSet(i)
    html += "<li>frameset nr=%i name=%s<br><ul>" % (i,frameset.name())
    for k in range( frameset.frameCount() ):
        frame = frameset.frame(k)
        html += "<li>frame nr=%i" % k
        html += " shapeId=%s" % frame.shapeId()
        html += " visible=%s" % frame.isVisible()
        #html += " rotation=%s" % frame.rotation()
        #html += " width=%s" % frame.width()
        #html += " height=%s" % frame.height()
        #html += " positionX=%s" % frame.positionX()
        #html += " positionY=%s" % frame.positionY()
        #html += " zIndex=%s" % frame.zIndex()
        html += "<br></li>"
    html += "</ul></li>"
html += "</ul>"

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertDefaultBlock()
cursor.insertHtml(html)
