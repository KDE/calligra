#include "waveletdecompose.h"
#include "kis_wdg_wavelet_decompose.h"
#include "kis_wavelet_decompose_filter.h"
#include <kpluginfactory.h>
#include <filter/kis_filter_registry.h>

K_PLUGIN_FACTORY(kritaWaveletFilterFactory, registerPlugin<WaveletDecomposePlugin>();)
K_EXPORT_PLUGIN(kritaWaveletFilterFactory("krita"))

WaveletDecomposePlugin::WaveletDecomposePlugin(QObject *parent, const QVariantList &) : QObject(parent)
{
    KisFilterRegistry::instance()->add(new KisWaveletDecomposeFilter());
}

WaveletDecomposePlugin::~WaveletDecomposePlugin()
{
}
