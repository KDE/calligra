#!/usr/bin/env python

import KWord, time

f = KWord.frameSet(0)
print "frameSet=%s" % f
d = f.textDocument()
print "textDocument=%s" % d

d.setHtml(
    (
        "<h1><font color=\"blue\">Test Python Script</font></h1>"
        "<ul>"
        "<li>Time=<b>%s</b></li>"
        "<li>pageCount=<b>%s</b></li>"
        "<li>frameSetCount=<b>%s</b></li>"
        "<li>document.url=<b>%s</b></li>"
        "<li>Title=<b>%s</b></li>"
        "<li>Subject=<b>%s</b></li>"
        "<li>Author=<b>%s</b></li>"
        "<li>EMail=<b>%s</b></li>"
        "<li>Keywords=<b>%s</b></li>"
        "</ul>"
    ) % (
        time.strftime('%H:%M.%S'),
        KWord.pageCount(),
        KWord.frameSetCount(),
        KWord.document().url(),
        KWord.document().documentInfoTitle(),
        KWord.document().documentInfoSubject(),
        KWord.document().documentInfoAuthorName(),
        KWord.document().documentInfoEmail(),
        KWord.document().documentInfoKeywords(),
    )
)
