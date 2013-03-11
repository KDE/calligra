#ifndef CALLIGRALIBS_KOABSTRACTGRADIENT_H
#define CALLIGRALIBS_KOABSTRACTGRADIENT_H

#include <KoResource.h>
#include <KoColor.h>
#include <QGradient>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>

class KoAbstractGradient : public KoResource
{
public:
    KoAbstractGradient(const QString &filename) : KoResource(filename), m_spreadMethod(QGradient::PadSpread), m_repeatType(QGradient::LinearGradient) {}

    virtual QGradient* toQGradient() const {
        QGradient *g = 0;
        switch (m_repeatType) {
            case QGradient::LinearGradient: g = new QLinearGradient(); break;
            case QGradient::RadialGradient: g = new QRadialGradient(); break;
            case QGradient::ConicalGradient: g = new QConicalGradient(); break;
            case QGradient::NoGradient: g = new QGradient(); break;
        }
        if (g) {
            g->setSpread(m_spreadMethod);
        }
        return g;
        
    }

    /// gets the color at position 0 <= t <= 1
    virtual void colorAt(KoColor&, qreal t) const {}

    void setColorSpace(KoColorSpace* colorSpace) {}
    const KoColorSpace * colorSpace() const { return 0; }

    void setSpread(QGradient::Spread spreadMethod) { m_spreadMethod = spreadMethod; }
    QGradient::Spread spread() const { return m_spreadMethod; }

    void setType(QGradient::Type repeatType) { m_repeatType = repeatType; }
    QGradient::Type type() const { return m_repeatType; }

    void updatePreview() {}
    QImage generatePreview(int width, int height) const { return image().scaled(width, height); }

private:
    QGradient::Spread m_spreadMethod;
    QGradient::Type m_repeatType;
};

#endif
