#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    virtual ~Settings();

    static Settings* instance();

signals:
    void openFileRequestedChanged(const QString &file);
    void valueChanged(const QString &name);

public slots:
    QString openFileRequested() const;
    void setOpenFileRequested(const QString &file);

    QVariantMap values() const;
    void setValues(const QVariantMap& values);

    bool hasValue(const QString &name) const;
    QVariant value(const QString &name, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &name, const QVariant &value);

    QVariantList valueList(const QString &name) const;
    void setValueList(const QString &name, const QVariantList &value);

    QVariantMap valueMap(const QString &name) const;
    void setValueMap(const QString &name, const QVariantMap &value);

    void saveChanges();

private:
    QVariantMap m_settings;
    bool m_modified;
    QString m_openFileRequested;

    void readGroup(QSettings &settings, QVariantMap &map);
    void writeGroup(QSettings &settings, QVariantMap &map);
};

#endif
