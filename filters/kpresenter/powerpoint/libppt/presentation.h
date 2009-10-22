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

#ifndef LIBPPT_PRESENTATION
#define LIBPPT_PRESENTATION


namespace Libppt
{

class Slide;
class TextFontCollection;
class TextFont;
class MainMasterContainer;
class TextCFException;
class TextPFException;

class Presentation
{
public:

    /*
     * Constructs a new presentation.
     */
    Presentation();

    /*
     * Destroys the presentation.
     */
    ~Presentation();

    /*
     * Clears the presentation, i.e. makes it as if it is just constructed.
     */
    void clear();

    /*
     * Loads the presentation file. Returns false if error occurred.
     */
    bool load(const char* filename);

    /*
     * Appends a new slide.
     */
    void appendSlide(Slide* slide);

    /*
     * Returns the number of slides in this presentation. A newly created
     * presentation has no slide, i.e. slideCount() returns 0.
     */
    unsigned slideCount() const;

    /*
     * Returns a slide at given index. If index is invalid (e.g. larger
     * than total number of slides), this function returns NULL.
     */

    Slide* slide(unsigned index);

    Slide* masterSlide();
    void setMasterSlide(Slide* master);

    /**
    * @brief Get specific font
    * @param index Index of the font to get
    * @return Specified font
    */
    TextFont* getFont(unsigned int index);

    /**
    * @brief Add a new font to collection
    * @param font Font to add
    */
    void addTextFont(const TextFont &font);

    /**
    * @brief Set MainMasterContainer for the presentation
    *
    * MainMasterContainer is a record that specifies a main master slide.
    *
    * This object assumes ownership of the container.
    */
    void setMainMasterContainer(MainMasterContainer *container);

    /**
    * @brief Get MainMasterContainer for this presentation
    * @return MainMasterContainer
    */
    MainMasterContainer *getMainMasterContainer();

    /**
    * @brief Set default character formatting
    *
    */
    void setTextCFDefaultsAtom(TextCFException *cf);

    /**
    * @brief set default paragraph formatting
    *
    */
    void setTextPFDefaultsAtom(TextPFException *pf);

    /**
    * @brief Get pointer to default paragraph exception
    * @return default paragraph exception
    */
    TextPFException *defaultTextPFException();

    /**
    * @brief Get pointer to default character exception
    * @return default character exception
    */
    TextCFException *defaultTextCFException();

private:
    // no copy or assign
    Presentation(const Presentation&);
    Presentation& operator=(const Presentation&);

    class Private;
    Private* d;
};


}

#endif /* LIBPPT_PRESENTATION */
