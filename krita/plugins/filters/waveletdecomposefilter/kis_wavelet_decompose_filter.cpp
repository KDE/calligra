#include "kis_wavelet_decompose_filter.h"
#include "kis_wdg_wavelet_decompose.h"

#include <KoCompositeOp.h>

#include "ui_wdg_wavelet_decompose.h"

#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>

KisWaveletDecomposeFilter::KisWaveletDecomposeFilter() : KisFilter(id(), categoryOther(), i18n("&Wavelet Decompose"))
{
    setSupportsPainting(true);
    setSupportsIncrementalPainting(true);
    setSupportsAdjustmentLayers(true);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

KisConfigWidget * KisWaveletDecomposeFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP) const
{
    return new KisWdgWaveletDecompose(parent);
}

KisFilterConfiguration* KisWaveletDecomposeFilter::factoryConfiguration(const KisPaintDeviceSP) const
{
    KisFilterConfiguration* config = new KisFilterConfiguration(id().id(), 1);

    config->setProperty("numberOfScales", 5);
    config->setProperty("layerRecomposition", true);
    config->setProperty("newImage", false);

    return config;
}

void KisWaveletDecomposeFilter::processImpl(KisPaintDeviceSP device,
                                        const QRect& rect,
                                        const KisFilterConfiguration* config,
                                        KoUpdater* progressUpdater
                                        ) const
{
    Q_UNUSED(device);
    Q_UNUSED(rect);
    Q_UNUSED(config);
    Q_UNUSED(progressUpdater);
}
