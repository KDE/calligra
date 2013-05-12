#ifndef GRAMMARCHECKPLUGIN_H
#define GRAMMARCHECKPLUGIN_H

#include <QObject>
#include <QVariant>

class GrammarCheckPlugin : public QObject
{
    Q_OBJECT

public:
    GrammarCheckPlugin(QObject *parent,  const QVariantList &);
};

#endif
