#include "KoAnnotationBalloon.h"

KoAnnotationBalloon::KoAnnotationBalloon(KoAnnotation content, int position = 0, QWidget *parent = 0):
        QWidget(parent), m_content(content), m_y(position)
{
    QGridLayout layout(this);
    setLayout(layout);
    m_textContent = new QTextEdit(m_content->content(), this);
    m_author = new QLabel(m_content->author, this);
    m_date = new QLabel((m_content->date()).toString("MM/dd/yyyy hh:mm"));

    layout.addWidget(m_textContent, 0, 0, 1, 2);
    layout.addItem(m_author, 1, 0);
    layout.addItem(m_date, 2, 0);

    m_optionButton = new QPushButton(this);
    m_options = new QMenu(this);
    // TODO: add menu items
    m_optionButton->setMenu(m_options);

    layout.addWidget(m_optionButton, 1, 1, 2, 1, Qt::AlignCenter);
}

void KoAnnotationBalloon::setFocus()
{
    m_textContent->setFocus();
}
