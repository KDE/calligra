#ifndef TESTDOCUMENT_H
#define TESTDOCUMENT_H

#include "KoDocumentBase.h"
#include <QImage>

class TestDocument : public KoDocumentBase
{
    Q_OBJECT
public:
    /// create a new TestDocument. It will be empty by default
    explicit TestDocument(KUndo2Stack *undoStack = new KUndo2Stack(), QObject *parent = 0);
    virtual ~TestDocument();

    virtual bool open(const QString &fileName);
    virtual bool saveTo(const QString &fileName);

    // TestDocument specific api

    /// return the loaded image
    QImage image() const;

signals:
    
    // emitted whenever the image is updates
    void updated();

public slots:
private:

    class Private;
    Private * const d;
    
};

class TestDocumentFactory : public KoDocumentFactory
{
    KoDocumentBase *create(KUndo2Stack *undoStack = new KUndo2Stack(), QObject *parent = 0);
};


#endif // TESTDOCUMENT_H
