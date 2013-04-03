#include "Settings.h"

#include <QStringList>
#include <qglobal.h>
#include <QDebug>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_modified(false)
{
    QSettings settings("org.kde.calligra.coffice");
    readGroup(settings, m_settings);
}

Settings::~Settings()
{
    saveChanges();
}

Q_GLOBAL_STATIC(Settings, settings)

Settings* Settings::instance()
{
    return settings();
}

QString Settings::openFileRequested() const
{
    return m_openFileRequested;
}

void Settings::setOpenFileRequested(const QString &file)
{
    if (m_openFileRequested == file)
        return;
    m_openFileRequested = file;
    Q_EMIT openFileRequestedChanged(m_openFileRequested);
}

QVariantMap Settings::values() const
{
    return m_settings;
}

void Settings::setValues(const QVariantMap& values)
{
    m_modified = true;
    m_settings = values;
    emit valueChanged(QString());
}

bool Settings::hasValue(const QString &name) const
{
    return m_settings.contains(name);
}

QVariant Settings::value(const QString &name, const QVariant &defaultValue) const
{
    QVariantMap::ConstIterator it = m_settings.constFind(name);
    return it == m_settings.constEnd() ? defaultValue : it.value();
}

void Settings::setValue(const QString &name, const QVariant &value)
{
    QVariantMap::Iterator it = m_settings.find(name);
    if (value.isNull() && it == m_settings.end())
        return;
    if (it != m_settings.end() && value == it.value())
        return;
    m_modified = true;
    if (value.isNull())
        m_settings.erase(it);
    else
        m_settings[name] = value;
    emit valueChanged(name);
}

QVariantList Settings::valueList(const QString &name) const
{
    QVariant v = value(name);
    if (!v.isValid())
        return QVariantList();
    return v.toList();
}

void Settings::setValueList(const QString &name, const QVariantList &value)
{
    setValue(name, value);
}

QVariantMap Settings::valueMap(const QString &name) const
{
    QVariant v = value(name);
    if (!v.isValid())
        return QVariantMap();
    return v.toMap();
}

void Settings::setValueMap(const QString &name, const QVariantMap &value)
{
    setValue(name, value);
}

void Settings::saveChanges()
{
    if (m_modified) {
        m_modified = false;
        qDebug() << "Saving settings now cause they got modified";

        QSettings settings("org.kde.calligra.coffice");
        settings.clear();
        writeGroup(settings, m_settings);
        settings.sync();
    }
}

void Settings::readGroup(QSettings &settings, QVariantMap &map)
{
    Q_FOREACH(const QString &key, settings.childKeys()) {
        QVariant v = settings.value(key);
        if (v.isNull())
            continue;
        map[key] = v;
    }
    Q_FOREACH(const QString &group, settings.childGroups()) {
        settings.beginGroup(group);
        QVariantMap groupMap;
        readGroup(settings, groupMap);
        if (!groupMap.isEmpty()) {
            map[group] = groupMap;
        }
        settings.endGroup();
    }
}

void Settings::writeGroup(QSettings &settings, QVariantMap &map)
{
    QStringList maps;
    for(QVariantMap::ConstIterator it = map.constBegin(); it != map.constEnd(); ++it) {
        QVariant v = it.value();
        if (it.value().type() == QVariant::Map) {
            maps.append(it.key());
        } else {
            settings.setValue(it.key(), it.value());
        }
    }
    Q_FOREACH(const QString &key, maps) {
        QVariantMap m = map[key].toMap();
        if (m.isEmpty())
            continue;
        settings.beginGroup(key);
        writeGroup(settings, m);
        settings.endGroup();
    }
}
