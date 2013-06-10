#ifndef KODOCUMENTCONTROLLER_H
#define KODOCUMENTCONTROLLER_H

#include <QObject>

/**
 * @brief The KoDocumentController class keeps track of all open documents, views and windows
 */
class KoDocumentController : public QObject
{
    Q_OBJECT
public:
    explicit KoDocumentController(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // KODOCUMENTCONTROLLER_H
