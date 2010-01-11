/* libppt - library to read PowerPoint presentation
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include "presentation.h"
#include "slide.h"
#include "powerpoint.h"
#include "objects.h"
#include <vector>

using namespace Libppt;

class Presentation::Private
{
public:
    Slide* masterSlide;
    std::vector<Slide*> slides;

    /**
    * @brief Collection of fonts shared by slides
    *
    */
    TextFontCollection fonts;

    /**
    * @brief A container record that specifies a main master slide.
    */
    MainMasterContainer *mainMasterContainer;

    /**
    * @brief An optional TextCFExceptionAtom record that specifies default
    * settings for character-level style and formatting.
    */
    TextCFException *textCFDefaultsAtom;

    /**
    * @brief An optional TextPFExceptionAtom record that specifies default
    * settings for paragraph-level style and formatting.
    */
    TextPFException *textPFDefaultsAtom;

    /**
     * @brief list of rgbuid for the images in this presentation
     * This data is obtained from the blipStore
     **/
    std::vector<std::string> bstore;
};

Presentation::Presentation()
{
    d = new Private;
    d->masterSlide = 0;
    d->mainMasterContainer = 0;
    d->textCFDefaultsAtom = 0;
    d->textPFDefaultsAtom = 0;
}

Presentation::~Presentation()
{
    clear();
    delete d;
}

void Presentation::clear()
{
    // FIXME use iterator
    for (unsigned i = 0; i < slideCount(); i++)
        delete slide(i);
    d->slides.clear();
    delete d->masterSlide;
    delete d->mainMasterContainer;
    delete d->textCFDefaultsAtom;
    delete d->textPFDefaultsAtom;

    d->mainMasterContainer = 0;
    d->textCFDefaultsAtom = 0;
    d->textPFDefaultsAtom = 0;
    d->masterSlide = 0;
    d->bstore.resize(0);
}

bool Presentation::load(const char* filename)
{
    PPTReader* reader = new PPTReader;
    bool result = reader->load(this, filename);
    delete reader;
    return result;
}

void Presentation::appendSlide(Slide* slide)
{
    d->slides.push_back(slide);
}

unsigned Presentation::slideCount() const
{
    return d->slides.size();
}

Slide* Presentation::slide(unsigned index)
{
    if (index >= slideCount()) return (Slide*)0;
    return d->slides[index];
}


Slide* Presentation::masterSlide()
{
    return d->masterSlide;
}

void Presentation::setMasterSlide(Slide* masterSlide)
{
    delete d->masterSlide;
    d->masterSlide = masterSlide;
}

TextFont* Presentation::getFont(unsigned index)
{
    return d->fonts.getFont(index);
}

void Presentation::addTextFont(const TextFont &font)
{
    d->fonts.addFont(font);
}

void Presentation::setMainMasterContainer(MainMasterContainer *container)
{
    d->mainMasterContainer = container;
}

MainMasterContainer *Presentation::getMainMasterContainer()
{
    return d->mainMasterContainer;
}

void Presentation::setTextCFDefaultsAtom(TextCFException *cf)
{
    if (!cf) {
        return;
    }

    if (d->textCFDefaultsAtom) {
        delete d->textCFDefaultsAtom;
    }

    d->textCFDefaultsAtom = new TextCFException(*cf);
}

void Presentation::setTextPFDefaultsAtom(TextPFException *pf)
{
    if (!pf) {
        return;
    }

    if (d->textPFDefaultsAtom) {
        delete d->textPFDefaultsAtom;
    }

    d->textPFDefaultsAtom = new TextPFException(*pf);
}

TextPFException *Presentation::defaultTextPFException()
{
    return d->textPFDefaultsAtom;
}

TextCFException *Presentation::defaultTextCFException()
{
    return d->textCFDefaultsAtom;
}

void Presentation::setBStore(const std::vector<std::string>& store)
{
    d->bstore = store;
}

const char* Presentation::getRgbUid(unsigned int bip) const
{
    return (d->bstore.size() > bip) ?d->bstore[bip].c_str() :0;
}
