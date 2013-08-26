#ifndef WAVELETDECOMPOSEPLUGIN_H_
#define WAVELETDECOMPOSEPLUGIN_H_

#include <QObject>
#include <QVariant>

class WaveletDecomposePlugin : public QObject
{
    Q_OBJECT
public:

    WaveletDecomposePlugin(QObject *parent, const QVariantList &);
    virtual ~WaveletDecomposePlugin();
};

#endif

