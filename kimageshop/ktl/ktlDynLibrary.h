#ifndef _DYNLIBRARY_H_INCLUDED
#define _DYNLIBRARY_H_INCLUDED

extern "C"
{
	#include <dlfcn.h>
}

#include <FC/Exceptions.h>


#define IMPLEMENT_DYNCREATE(className) void *__handle_ ## className() { \
                                         return new className; \
                                       }
DECLARE_EXCEPTION(LibraryNotFoundException)
DECLARE_EXCEPTION(ClassNotFoundException)

class DynLibrary
{	 
 public:
  
   typedef enum {
      LN_LAZY   = RTLD_LAZY,
      LN_NOW    = RTLD_NOW,
      LN_GLOBAL = RTLD_GLOBAL
   } ResolveType;
   
 public:
   DynLibrary(const string &name, DynLibrary::ResolveType option = LN_NOW);
   virtual ~DynLibrary();
   
   bool isOpen();
   
   void openLibrary()
      throw(LibraryNotFoundException);
   void closeLibrary();
   
   void *loadClass(const string &className )
      throw(ClassNotFoundException);
   
 protected:
   string       libName;
   ResolveType  resolveOptions;
   void        *libHandle;
};    

#endif // _DYNLIBRARY_H_INCLUDED
      
