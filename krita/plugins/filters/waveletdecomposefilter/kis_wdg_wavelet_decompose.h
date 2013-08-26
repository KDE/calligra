#ifndef _KIS_WDG_WAVELET_DECOMPOSE_H_
#define _KIS_WDG_WAVELET_DECOMPOSE_H_

#include <kis_config_widget.h>

class KisFilter;
class Ui_WdgWaveletDecompose;

class KisWdgWaveletDecompose : public KisConfigWidget
{
    Q_OBJECT
public:
    KisWdgWaveletDecompose(QWidget* parent);
    virtual ~KisWdgWaveletDecompose();

    inline const Ui_WdgWaveletDecompose* widget() const {
        return m_widget;
    }

    virtual void setConfiguration(const KisPropertiesConfiguration*);
    virtual KisPropertiesConfiguration* configuration() const;

private:
    Ui_WdgWaveletDecompose* m_widget;
};

#endif
