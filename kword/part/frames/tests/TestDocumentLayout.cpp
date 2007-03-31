#include "TestDocumentLayout.h"

#include <KWTextFrame.h>
#include <KWTextFrameSet.h>
#include <KWTextDocumentLayout.h>

#include <KoParagraphStyle.h>
#include <KoListStyle.h>
#include <KoTextBlockData.h>
#include <KoStyleManager.h>

#include <QtGui>

#include <kdebug.h>
#include <kcomponentdata.h>

#define ROUNDING 0.126
#define FRAME_SPACING 10.0


void TestDocumentLayout::initTestCase() {
    frameSet = 0;
    shape1 = 0;
    doc = 0;
    layout = 0;
    blockLayout = 0;

    loremIpsum = QString("Lorem ipsum dolor sit amet, XgXgectetuer adiXiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.");
}

void TestDocumentLayout::initForNewTest(const QString &initText) {
    // this leaks memory like mad, but who cares ;)
    frameSet = new KWTextFrameSet(0);
    shape1 = new MockTextShape();
    shape1->resize(QSizeF(200, 1000));
    new KWTextFrame(shape1, frameSet);
    doc = frameSet->document();
    Q_ASSERT(doc);
    layout = dynamic_cast<KWTextDocumentLayout*> (doc->documentLayout());
    Q_ASSERT(layout);
    styleManager = new KoStyleManager();
    layout->setStyleManager(styleManager);

    QTextBlock block = doc->begin();
    if(initText.length() > 0) {
        QTextCursor cursor(doc);
        cursor.insertText(initText);
        KoParagraphStyle style;
        style.setStyleId(101); // needed to do manually since we don't use the stylemanager
        QTextBlock b2 = doc->begin();
        while(b2.isValid()) {
            style.applyStyle(b2);
            b2 = b2.next();
        }
    }
    blockLayout = block.layout();
}

QTEST_KDEMAIN(TestDocumentLayout, GUI)

#include "TestDocumentLayout.moc"
