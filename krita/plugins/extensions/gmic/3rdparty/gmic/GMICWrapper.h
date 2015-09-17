#ifndef GMICWRAPPER_H
#define GMICWRAPPER_H

/// A wrapper around g'mic to make sure Qt includes and CImg.h don't mix.

#include <string>

// gmic_image<float>
class GMICImageWrapper {
public:
    GMICImageWrapper();
    virtual ~GMICImageWrapper();
    GMICImageWrapper(const GMICImageWrapper &rhs);
    GMICImageWrapper operator=(const GMICImageWrapper &rhs);

    unsigned int spectrum() const;
    unsigned int width() const;
    unsigned int height() const;
    unsigned int depth() const;
    float *data() const;
    void assign(unsigned int x, unsigned int y, unsigned int z, unsigned int depth) const;

    struct Private;
    Private *d;
};

//gmic_list<float>
class GMICImageWrapperList {
public:
    GMICImageWrapperList();
    virtual ~GMICImageWrapperList();
    GMICImageWrapper data(int index) const;
    void assign(int size);

    struct Private;
    Private *d;
};


namespace GMICWrapper
{
    int version();
    void saveLibrary(const char *src, const char *dst);
    const char *doGmic(const char *cmd, GMICImageWrapperList &images,
                             const char *customCommands,
                             bool includeDefaultCommands,
                             bool *cancelPtr,
                             float *progressPtr);

};


#endif // GMICWRAPPER_H
