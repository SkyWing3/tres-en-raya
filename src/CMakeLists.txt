add_subdirectory(core)
add_subdirectory(presentation)

if(TRESENRAYA_BUILD_APP)
    add_subdirectory(app)
endif()
