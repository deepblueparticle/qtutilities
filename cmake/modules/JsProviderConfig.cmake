# determines the JavaScript provider (either Qt Script or Qt Declarative)

if(TARGET_CONFIG_DONE)
    message(FATAL_ERROR "Can not include JsProviderConfig module when targets are already configured.")
endif()

include(QtLinkage)

set(JS_PROVIDER "auto" CACHE STRING "specifies the JavaScript provider: auto (default), qml, script or none")
if(NOT JS_PROVIDER OR "${JS_PROVIDER}" STREQUAL "auto")
    find_qt5_module(Script OPTIONAL)
    if(QT5_Script_FOUND)
        set(JS_PROVIDER Script)
        set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_USE_SCRIPT")
        list(APPEND ADDITIONAL_QT_REPOS "script")
        message(STATUS "No JavaScript provider explicitly specified, defaulting to Qt Script.")
    else()
        find_qt5_module(Qml OPTIONAL)
        if(QT5_Qml_FOUND)
            set(JS_PROVIDER Qml)
            set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_USE_JSENGINE")
            list(APPEND ADDITIONAL_QT_REPOS "declarative")
            message(STATUS "No JavaScript provider explicitly specified, defaulting to Qt QML.")
        else()
            set(JS_PROVIDER "")
            set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_NO_JSENGINE")
            message(STATUS "No JavaScript provider available, features requiring JavaScript have been disabled.")
        endif()
    endif()
else()
    if(${JS_PROVIDER} STREQUAL "script")
        find_qt5_module(Script REQUIRED)
        set(JS_PROVIDER Script)
        set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_USE_SCRIPT")
        list(APPEND ADDITIONAL_QT_REPOS "script")
        message(STATUS "Using Qt Script as JavaScript provider.")
    elseif(${JS_PROVIDER} STREQUAL "qml")
        find_qt5_module(Qml REQUIRED)
        set(JS_PROVIDER Qml)
        set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_USE_JSENGINE")
        list(APPEND ADDITIONAL_QT_REPOS "declarative")
        message(STATUS "Using Qt QML as JavaScript provider.")
    elseif(${JS_PROVIDER} STREQUAL "none")
        set(JS_PROVIDER "")
        set(JS_DEFINITION "${META_PROJECT_VARNAME_UPPER}_NO_JSENGINE")
        message(STATUS "JavaScript provider has been disabled.")
    else()
        message(FATAL_ERROR "The specified JavaScript provider '${JS_PROVIDER}' is unknown.")
    endif()
endif()

if(JS_PROVIDER)
    use_qt5_module(${JS_PROVIDER} REQUIRED)

    # add header files with some defines/includes to conveniently use the selected provider
    if(META_JS_SRC_DIR)
        set(JS_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${META_JS_SRC_DIR}")
    else()
        set(JS_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gui")
    endif()
    include(TemplateFinder)
    find_template_file("jsdefs.h" QT_UTILITIES JS_DEFS_H_TEMPLATE_FILE)
    configure_file(
        "${JS_DEFS_H_TEMPLATE_FILE}"
        "${JS_HEADER_DIR}/jsdefs.h" # simply add this to source to ease inclusion
        NEWLINE_STYLE UNIX # since this goes to sources ensure consistency
    )
    find_template_file("jsincludes.h" QT_UTILITIES JS_INCLUDES_H_TEMPLATE_FILE)
    configure_file(
        "${JS_INCLUDES_H_TEMPLATE_FILE}"
        "${JS_HEADER_DIR}/jsincludes.h" # simply add this to source to ease inclusion
        NEWLINE_STYLE UNIX # since this goes to sources ensure consistency
    )
    list(APPEND WIDGETS_FILES
        "${JS_HEADER_DIR}/jsdefs.h"
        "${JS_HEADER_DIR}/jsincludes.h"
    )
endif()

list(APPEND META_PUBLIC_COMPILE_DEFINITIONS ${JS_DEFINITION})
