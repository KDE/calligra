#ifndef PRESENTATIONVIEWPORTSHAPEPLUGIN_H
#define PRESENTATIONVIEWPORTSHAPEPLUGIN_H

#include <QObject>
#include <QVariantList>

class PresentationViewPortShapePlugin : public QObject
{
    Q_OBJECT

public:
    PresentationViewPortShapePlugin( QObject * parent, const QVariantList & );
    ~PresentationViewPortShapePlugin();
};

#endif