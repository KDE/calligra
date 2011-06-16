#include "KoAnnotation.h"

KoAnnotation::KoAnnotation(QString author, QDateTime date, QTextBlock content, QObject *parent = 0) :
    QObject(parent), m_author(author), m_date(date), m_content(content)
{
}
