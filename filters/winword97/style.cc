#include <style.h>

CStyle::CStyle() {
    formatDirty=true;
}

CStyle::CStyle(CStyle &rhs) {

    if(!rhs.dirty()) {
        //theFormat=rhs.format();
        formatDirty=false;
    }
}
