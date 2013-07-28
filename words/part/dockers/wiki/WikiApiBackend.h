#ifndef WIKIAPIBACKEND_H
#define WIKIAPIBACKEND_H

#include <QObject>

class WikiApiBackend : public QObject
{
    Q_OBJECT
public:
    explicit WikiApiBackend();
    
    bool login(QString &username, QString &password);
signals:
    
public slots:
    
};

#endif // WIKIAPIBACKEND_H
