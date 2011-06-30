#ifndef TESTEDITCUSTOMSLIDESHOWSCOMMAND_H
#define TESTEDITCUSTOMSLIDESHOWSCOMMAND_H

#include <QtTest/QtTest>

class TestEditCustomSlideShowsCommand : public QObject
{
    Q_OBJECT
private slots:
    //Just test one edit action because the command stores old and new list in the same
    //way for all actions.
    void moveSingleSlide();

};
#endif // TESTEDITCUSTOMSLIDESHOWSCOMMAND_H
