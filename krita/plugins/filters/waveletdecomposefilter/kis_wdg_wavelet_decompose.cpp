#include "filter/kis_filter.h"
#include "filter/kis_filter_configuration.h"

#include "kis_wdg_wavelet_decompose.h"
#include "ui_wdg_wavelet_decompose.h"

KisWdgWaveletDecompose::KisWdgWaveletDecompose(QWidget* parent) : KisConfigWidget(parent)
{
    m_widget = new Ui_WdgWaveletDecompose();
    m_widget->setupUi(this);

    connect(m_widget->waveletScaleSpinbox, SIGNAL(valueChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->newImageCheckbox, SIGNAL(toggled(bool)), SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->setLayerCheckbox, SIGNAL(toggled(bool)), SIGNAL(sigConfigurationItemChanged()));
}

KisWdgWaveletDecompose::~KisWdgWaveletDecompose()
{
    delete m_widget;
}

KisPropertiesConfiguration* KisWdgWaveletDecompose::configuration() const
{
    KisFilterConfiguration *config = new KisFilterConfiguration("wavelet decompose",1);
    config->setProperty("numberOfScales", m_widget->waveletScaleSpinbox->value());
    config->setProperty("layerRecomposition", m_widget->setLayerCheckbox->isChecked());
    config->setProperty("newImage", m_widget->newImageCheckbox->isChecked());
    return config;
}

void KisWdgWaveletDecompose::setConfiguration(const KisPropertiesConfiguration* config)
{
    QVariant value;
    if (config->getProperty("numberOfScales", value)) {
        m_widget->waveletScaleSpinbox->setValue(value.toUInt());
    }
    if (config->getProperty("layerRecomposition", value)) {
        m_widget->setLayerCheckbox->setChecked(value.toBool());
    }
    if (config->getProperty("newImage", value)) {
        m_widget->newImageCheckbox->setChecked(value.toBool());
    }
    
}

#include "kis_wdg_wavelet_decompose.moc"
