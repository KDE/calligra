#include "kis_lindenmayer_production.h"

#include "kis_lindenmayer_letter.h"

#include <QDebug>

KisLindenmayerProduction::KisLindenmayerProduction()
{
}

// angles are relative to the vertical axis, +90 degrees points to right, -90 to left. positive +90+180=+270 points to the left. watch out, what happens at +180 or -180

//default parameters:
// position, angle, length, age

// computed parameters:
// endPosition, angleToSun (points towards the mouse position), distanceToSun

QList<KisLindenmayerLetter*> KisLindenmayerProduction::produce(KisLindenmayerLetter* letter) {
    QList<KisLindenmayerLetter*> retList;
//    letter->setParameter("angle", letter->getComputedParameter("angleToSun").toFloat());
//    letter->setParameter("length", 100);
//    qDebug() << "letter angle:" << letter->getParameter("angle");
//    qDebug() << "letter position: " << letter->getParameter("position");
//    qDebug() << "letter end position: " << letter->getComputedParameter("endPosition");
//    qDebug() << "letter angle from end to sun: " << letter->getComputedParameter("angleToSun");



//    if(letter->length() < 20.0f && letter->getParameter("branched").toBool() == false) {
//        if(letter->getComputedParameter("distanceToSun").toFloat() < 50) {// for safty reasons..
//            letter->setParameter("length", letter->length()+letter->getComputedParameter("distanceToSun").toFloat()/20.0);
//            letter->setParameter("drawn", false);
//        }
//    }

    if(/*letter->length() > 14.0f
            && */
            letter->getComputedParameter("distanceToSun").toFloat() > 50.0f
            && letter->getParameter("branched").toBool() == false
            && letter->getParameter("stem").toBool() == false
            && letter->getParameter("leaf").toBool() == false) {
        KisLindenmayerLetter* newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 30.0f);
        newLetter->setParameter("angle", newLetter->getComputedParameter("angleToSun"));
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        retList.append(newLetter);

        letter->setParameter("branched", true);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 21.0f);
        newLetter->setParameter("angle", newLetter->getComputedParameter("angleToSun").toFloat() - 45.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("stem", true);
        retList.append(newLetter);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 21.0f);
        newLetter->setParameter("angle", newLetter->getComputedParameter("angleToSun").toFloat() + 45.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("stem", true);
        retList.append(newLetter);
    }

    if(letter->getParameter("stem").toBool() == true && letter->getParameter("leaf").toBool() == false) {
        KisLindenmayerLetter* newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 10.0f);
        newLetter->setParameter("angle", letter->getParameter("angle"));
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("leaf", true);
        retList.append(newLetter);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 10.0f);
        newLetter->setParameter("angle", letter->getParameter("angle").toFloat() + 15.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("leaf", true);
        retList.append(newLetter);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 10.0f);
        newLetter->setParameter("angle", letter->getParameter("angle").toFloat() + 30.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("leaf", true);
        retList.append(newLetter);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 10.0f);
        newLetter->setParameter("angle", letter->getParameter("angle").toFloat() - 15.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("leaf", true);
        retList.append(newLetter);

        newLetter = letter->createCopy();
        newLetter->setParameter("drawn", false);
        newLetter->setParameter("length", 10.0f);
        newLetter->setParameter("angle", letter->getParameter("angle").toFloat() - 30.0f);
        newLetter->setParameter("position", letter->getComputedParameter("endPosition"));
        newLetter->setParameter("leaf", true);
        retList.append(newLetter);
    }

    if(letter->getParameter("drawn").toBool() == true && (letter->getParameter("leaf").toBool() == true
                                                          || letter->getParameter("stem").toBool() == true
                                                          || letter->getParameter("branched").toBool() == true)) {
        delete letter;
    }
    else {
        retList.append(letter);
    }

    return retList;
}
