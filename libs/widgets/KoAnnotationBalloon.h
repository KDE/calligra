#ifndef KOANNOTATIONBALLOON_H
#define KOANNOTATIONBALLOON_H

#include <KoBalloon.h>
#include <KoAnnotation.h>

class KoAnnotationBalloon : public KoBalloon
{
public:
    KoAnnotationBalloon(KoAnnotation content, int position = 0, QWidget *parent = 0);

public slots:
    // override setFocus, gives focus to m_textContent
    void setFocus();

private:
    KoAnnotation *m_content;
    QTextEdit *m_textContent;
    QLabel *m_author;
    QLabel *m_date;

    QPushButton *optionButton;
    QMenu *options;
};

#endif // KOANNOTATIONBALLOON_H
