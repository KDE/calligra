#include <koFilter.h>


KoFilter::KoFilter(KoFilter *parent, QString name) : QObject(parent, name) {
}

const bool KoFilter::filter(const QCString &, const QCString &,
			    const QCString &, const QCString &,
			    const QString &) {
    return false;
}

const bool KoFilter::filter(const QCString &, const QDomDocument *,
			    const QCString &, const QCString &,
			    const QString &) {
    return false;
}

const bool KoFilter::filter(const QCString &, const KoStore *,
			    const QCString &, const QCString &,
			    const QString &) {
    return false;
}

const bool KoFilter::I_filter(const QCString &, KoDocument * const,
			      const QCString &, const QCString &,
			      const QString &) {
    return false;
}

const bool KoFilter::E_filter(const QCString &, const KoDocument * const,
			      const QCString &, const QCString &,
			      const QString &) {
    return false;
}
#include <koFilter.moc>
