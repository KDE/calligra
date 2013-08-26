#ifndef KIS_WAVELET_DECOMPOSE_FILTER_H
#define KIS_WAVELET_DECOMPOSE_FILTER_H

#include "filter/kis_filter.h"
#include "ui_wdg_wavelet_decompose.h"

#include <Eigen/Core>

using namespace Eigen;

class KisWaveletDecomposeFilter : public KisFilter
{
public:
    KisWaveletDecomposeFilter();
public:
    using KisFilter::process;

    void processImpl(KisPaintDeviceSP device,
                     const QRect& rect,
                     const KisFilterConfiguration* config,
                     KoUpdater* progressUpdater
                     ) const;
    static inline KoID id() {
        return KoID("wavelet decompose", i18n("Wavelet Decompose"));
    }

    virtual KisFilterConfiguration* factoryConfiguration(const KisPaintDeviceSP) const;
public:
    KisConfigWidget * createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev) const;

};

#endif

