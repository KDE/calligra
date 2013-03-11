#ifndef KORESOURCE_H
#define KORESOURCE_H

#include <QImage>
#include <QString>
#include <QFileInfo>

class KoResource
{
public:
    explicit KoResource(const QString &filename) : m_filename(filename), m_valid(false) {}
    virtual ~KoResource() {}

    virtual bool load() { return true; }
    virtual bool save() { return true; }

    virtual QImage image() const { return m_image; }
    void setImage(const QImage& image) { m_image = image; }

    qint32 width() const { return image().size().width(); }
    qint32 height() const { return image().size().height(); }

    bool removable() const { return false; }
    virtual QString defaultFileExtension() const { return QString(); }

    QString filename() const { return m_filename; }
    void setFilename(const QString& filename) { m_filename = filename; }

    QString shortFilename() const { return QFileInfo(m_filename).fileName(); }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    bool valid() const { return m_valid; }
    void setValid(bool valid) { m_valid = valid; }

private:
    QString m_filename;
    QString m_name;
    QImage m_image;
    bool m_valid;
};

#endif

