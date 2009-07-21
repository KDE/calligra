project(kpresenter)

include(CheckPrototypeExists)

check_prototype_exists(DPMSCapable "X11/Xlib.h;X11/extensions/dpms.h" HAVE_DPMSCAPABLE_PROTO)
macro_bool_to_01(X11_dpms_FOUND HAVE_DPMS) # kpresenter

add_subdirectory( part )

include_directories(
		${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
)

add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( styles )
#add_subdirectory( dtd )
#add_subdirectory( slideshow )
