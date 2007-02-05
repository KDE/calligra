#!/usr/bin/env python

import KWord, time, sys, os

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

# Set the default cascading stylesheet.
doc.setDefaultStyleSheet(
    (
        "li { margin-left:1em; }"
    )
)

# Set the html with some general informations.
doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Sample: Text</font></h1>"
        "<p><i>italic</i> and <b>bold</b> and <u>underlined</u> and a <a href=\"test\">link</a></p>."
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

        ','.join(KWord.application().getDocuments()),
        ','.join(KWord.application().getViews()),
        ','.join(KWord.application().getWindows()),

        KWord.pageCount(),
        KWord.frameSetCount(),
        KWord.frameCount(),

        KWord.document().url(),

        KWord.document().documentInfoTitle(),
        KWord.document().documentInfoSubject(),
        KWord.document().documentInfoKeywords(),
        KWord.document().documentInfoAbstract(),

        KWord.document().documentInfoAuthorName(),
        KWord.document().documentInfoCompanyName(),
        KWord.document().documentInfoAuthorPostion(),
        KWord.document().documentInfoEmail(),
        KWord.document().documentInfoTelephone(),
        KWord.document().documentInfoTelephoneWork(),
        KWord.document().documentInfoTelephoneHome(),
        KWord.document().documentInfoFax(),
        KWord.document().documentInfoCountry(),
        KWord.document().documentInfoPostalCode(),
        KWord.document().documentInfoCity(),
        KWord.document().documentInfoStreet(),
        KWord.document().documentInfoInitial(),
    )
)

# Add a list of details about the framesets and frames.
html = "<p><b>Framesets:</b></p><ul>"
for i in range( KWord.frameSetCount() ):
    frameset = KWord.frameSet(i)
    html2 = "<li>nr=%i name=%s<br><ul>" % (i,frameset.name())
    for k in range( frameset.frameCount() ):
        frame = frameset.frame(k)
        html2 += "<li>nr=%i" % k
        html2 += " shapeId=%s" % frame.shapeId()
        html2 += " visible=%s" % frame.isVisible()
        #html2 += " scaleX=%s" % frame.scaleX()
        #html2 += " scaleY=%s" % frame.scaleY()
        #html2 += " rotation=%s" % frame.rotation()
        #html2 += " shearX=%s" % frame.shearX()
        #html2 += " shearY=%s" % frame.shearY()
        #html2 += " width=%s" % frame.width()
        #html2 += " height=%s" % frame.height()
        #html2 += " positionX=%s" % frame.positionX()
        #html2 += " positionY=%s" % frame.positionY()
        #html2 += " zIndex=%s" % frame.zIndex()
        html2 += "<br></li>"
    html += "%s</ul></li>" % html2
html += "</ul>"

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertDefaultBlock()
cursor.insertHtml(html)
