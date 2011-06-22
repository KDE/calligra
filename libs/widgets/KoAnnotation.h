#ifndef KOANNOTATION_H
#define KOANNOTATION_H

#include <QObject>

class KoAnnotation : public QObject
{
    Q_OBJECT
public:
    explicit KoAnnotation(QString author, QDateTime date, QString content, QObject *parent = 0);
    QString author(){return m_author;}
    QDateTime date(){return m_date;}
    QString content(){return m_content;}

private:
    QString m_author;
    QDateTime m_date;
    QString m_content;

};

#endif // KOANNOTATION_H
