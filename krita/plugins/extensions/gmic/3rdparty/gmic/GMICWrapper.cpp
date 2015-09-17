#include "GMICWrapper.h"

#include <CImg.h>
#include <gmic.h>

int GMICWrapper::version()
{
    return gmic_version;
}


void GMICWrapper::saveLibrary(const char *src, const char *dst)
{

    std::FILE *file = std::fopen(src,"rb");
    cimg_library::CImg<unsigned char> buffer;
    buffer.load_cimg(file);
    buffer.save_raw(dst);
    std::fclose(file);

}


struct GMICImageWrapper::Private {
    gmic_image<float> gmicImage;
};

GMICImageWrapper::GMICImageWrapper()
    : d(new Private)
{
}

GMICImageWrapper::~GMICImageWrapper()
{
    delete d;
}

GMICImageWrapper::GMICImageWrapper(const GMICImageWrapper &rhs)
{
    d->gmicImage = rhs.d->gmicImage;
}

GMICImageWrapper GMICImageWrapper::operator=(const GMICImageWrapper &rhs)
{
    GMICImageWrapper wrap;
    wrap.d->gmicImage = rhs.d->gmicImage;
    return wrap;
}

unsigned int GMICImageWrapper::spectrum() const
{
    return d->gmicImage._spectrum;
}

unsigned int GMICImageWrapper::width() const
{
    return d->gmicImage._width;
}

unsigned int GMICImageWrapper::height() const
{
    return d->gmicImage._height;
}

unsigned int GMICImageWrapper::depth() const
{
    return d->gmicImage._depth;
}

float *GMICImageWrapper::data() const
{
    return d->gmicImage._data;
}

void GMICImageWrapper::assign(unsigned int x, unsigned int y, unsigned int z, unsigned int depth) const
{
    d->gmicImage.assign(x, y, z, depth);
}

struct GMICImageWrapperList::Private {
    gmic_list<float> images;
};

GMICImageWrapperList::GMICImageWrapperList()
    : d(new Private)
{

}

GMICImageWrapperList::~GMICImageWrapperList()
{
    delete d;
}

GMICImageWrapper GMICImageWrapperList::data(int index) const
{
    GMICImageWrapper wrapper;
    wrapper.d->gmicImage = d->images[index];
    return wrapper;
}

void GMICImageWrapperList::assign(int size)
{
    d->images.assign(size);
}


const char *GMICWrapper::doGmic(const char *cmd, GMICImageWrapperList &images,
                         const char *customCommands,
                         bool includeDefaultCommands,
                         bool *cancelPtr,
                         float *progressPtr)
{
    gmic_list<char> images_names; // unused
    try {
        gmic(cmd, images.d->images, images_names, customCommands, includeDefaultCommands, progressPtr, cancelPtr);
        return 0;
    }
    catch (gmic_exception &e)  {
        return e.what();
    }

}
