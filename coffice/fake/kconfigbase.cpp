#include <kconfigbase.h>
#include <kconfiggroup.h>

KConfigBase::KConfigBase(const QString &path) : m_path(path) {}

QStringList KConfigBase::groupList() const { return QStringList(); }

bool KConfigBase::hasGroup(const QString &group) const { return false; }
bool KConfigBase::hasGroup(const char *group) const { return false; }
bool KConfigBase::hasGroup(const QByteArray &group) const { return false; }

KConfigGroup KConfigBase::group(const QByteArray &group) { return KConfigGroup(m_path + "/" + group); }
KConfigGroup KConfigBase::group(const QString &group) { return KConfigGroup(m_path + "/" + group.toUtf8()); }
KConfigGroup KConfigBase::group(const char *group) { return KConfigGroup(m_path + "/" + group); }

const KConfigGroup KConfigBase::group(const QByteArray &group) const { return KConfigGroup(m_path + "/" + group); }
const KConfigGroup KConfigBase::group(const QString &group) const { return KConfigGroup(m_path + "/" + group.toUtf8()); }
const KConfigGroup KConfigBase::group(const char *group) const { return KConfigGroup(m_path + "/" + group); }
