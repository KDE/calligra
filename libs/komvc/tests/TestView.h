#ifndef TESTVIEW_H
#define TESTVIEW_H

#include "KoViewBase.h"

class KoDocumentBase;


class TestView : public KoViewBase
{
    Q_OBJECT
public:
    explicit TestView(KoDocumentBase *document, QWidget *parent = 0);
    
signals:
    
private slots:

    void redisplay() const;

private:

    class Private;
    Private *const d;
};

class TestViewFactory : public KoViewFactory {
public:
    virtual ~TestViewFactory() {}
    virtual KoViewBase *create(KoDocumentBase *document, QWidget *parent = 0) {
        return new TestView(document, parent);
    }
};

#endif // TESTVIEW_H
