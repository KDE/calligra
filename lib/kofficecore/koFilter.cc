#include <koFilter.h>


KoFilter::KoFilter(KoFilter *parent, QString name) : QObject(parent, name) {
}

const bool KoFilter::filter(const QCString &, const QCString &,
			    const QCString &, const QCString &,
			    const QString &) {
    return false;
}

const QDomDocument *KoFilter::I_filter(const QCString &, const QCString &,
				       const QCString &, const QString &) {
    return 0L;
}

const bool KoFilter::I_filter(const QCString &, KoDocument *,
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
