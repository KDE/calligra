#ifndef KGRAPH_PART_H
#define KGRAPH_PART_H

#include <koDocument.h>

class QPainter;


class KGraphPart : public KoDocument {

    Q_OBJECT

public:
    KGraphPart(QObject *parent=0, const char *name=0, bool singleViewMode=false);

    virtual KoView *createView(QWidget *parent=0, const char *name=0);
    virtual KoMainWindow *createShell();

    virtual void paintContent(QPainter &painter, const QRect &rect, bool transparent=false);

    virtual bool initDoc();

    virtual QCString mimeType() const;

    // (TODO) add a virtual CTOR which creates the objects for the
    // ObjectPool from XML (=loading)

protected slots:
    void a_editcut();   // a_ ... Action
};
#endif
