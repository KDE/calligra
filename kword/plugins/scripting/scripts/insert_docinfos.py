#!/usr/bin/env python

import KWord

class Items:
    def __init__(self):
        self.items = []
        self.addItems()

    def addItem(self, caption, value):
        if value != None and str(value) != "":
            self.items.append("<li>%s: <b>%s</b></li>" % (caption,value))

    def addItems(self):
        import KWord, os, sys, time

        self.addItem("Title", KWord.document().documentInfoTitle())
        self.addItem("Subject", KWord.document().documentInfoSubject())
        self.addItem("Url", KWord.document().url())
        self.addItem("Keywords", KWord.document().documentInfoKeywords())
        self.addItem("Abstract", KWord.document().documentInfoAbstract())

        self.addItem("Documents", ','.join(KWord.application().getDocuments()))
        self.addItem("Views", ','.join(KWord.application().getViews()))
        self.addItem("Windows", ','.join(KWord.application().getWindows()))

        self.addItem("Author Name", KWord.document().documentInfoAuthorName())
        self.addItem("Author Company", KWord.document().documentInfoCompanyName())
        self.addItem("Author Postion", KWord.document().documentInfoAuthorPostion())
        self.addItem("Author Email", KWord.document().documentInfoEmail())
        self.addItem("Author Telephone", KWord.document().documentInfoTelephone())
        self.addItem("Author Telephone Work", KWord.document().documentInfoTelephoneWork())
        self.addItem("Author Telephone Home", KWord.document().documentInfoTelephoneHome())
        self.addItem("Author Fax", KWord.document().documentInfoFax())
        self.addItem("Author Country", KWord.document().documentInfoCountry())
        self.addItem("Author Postal Code", KWord.document().documentInfoPostalCode())
        self.addItem("Author City", KWord.document().documentInfoCity())
        self.addItem("Author Street", KWord.document().documentInfoStreet())
        self.addItem("Author Initial", KWord.document().documentInfoInitial())

        self.addItem("Date and time", time.strftime('%Y-%m-%d %H:%M'))
        self.addItem("Operating System", ' '.join(os.uname()))
        self.addItem("Python Version", sys.version)

        self.addItem("Page Count", KWord.pageCount())
        self.addItem("FrameSet Count", KWord.frameSetCount())
        self.addItem("Frame Count", KWord.frameCount())

items = Items()

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()
doc.setHtml(
    (
        "<ul>"
        "%s"
        "</ul>"
    ) % "\n".join(items.items)
)
