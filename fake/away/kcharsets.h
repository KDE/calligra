#ifndef FAKE_KCHARSETS_H
#define FAKE_KCHARSETS_H

#include <QTextCodec>

#include "kofake_export.h"


class KCharsets
{
public:
    static KCharsets* charsets();
    QTextCodec *codecForName(const QString &name) const { return QTextCodec::codecForName(name.toUtf8()); }
    QTextCodec *codecForName(const QString &name, bool &ok) const { QTextCodec *c = QTextCodec::codecForName(name.toUtf8()); ok = c; return c; }
    static QChar fromEntity(const QString &str) { return QChar(); }
    static QChar fromEntity(const QString &str, int &len) { return QChar(); }
    static QString toEntity(const QChar &ch) { return QString(); }
    static QString resolveEntities( const QString &text ) { return QString(); }
    QStringList availableEncodingNames() const {
        QStringList r;
        Q_FOREACH(const QByteArray &b, QTextCodec::availableCodecs())
            r << QString::fromUtf8(b);
        return r;
    }
    QStringList descriptiveEncodingNames() const { return availableEncodingNames(); }
    QList<QStringList> encodingsByScript() const { return QList<QStringList>(); }
    QString descriptionForEncoding( const QString& encoding ) const { return QString(); }
    QString encodingForName( const QString &descriptiveName ) const { return QString(); }
};

Q_GLOBAL_STATIC(KCharsets, globalKCharsets)

namespace KGlobal
{
    static KCharsets *charsets() { return globalKCharsets(); }
}

KCharsets* KCharsets::charsets() { return globalKCharsets(); }

#endif
