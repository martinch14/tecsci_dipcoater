# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(dipcoater_esp2)






if(IDF_VERSION_MAJOR GREATER_EQUAL 4)
    idf_component_register(SRC_DIRS components/wifi-manager
        REQUIRES log nvs_flash mdns wpa_supplicant lwip esp_http_server
        INCLUDE_DIRS components/wifi-manager/include
        EMBED_FILES components/wifi-manager/style.css components/wifi-manager/jquery.gz components/wifi-manager/code.js components/wifi-manager/index.html)
else()
    set(COMPONENT_SRCDIRS components/wifi-manager)
    set(COMPONENT_ADD_INCLUDEDIRS components/wifi-manager)
    set(COMPONENT_REQUIRES log nvs_flash mdns wpa_supplicant lwip esp_http_server)
    set(COMPONENT_EMBED_FILES components/wifi-manager/style.css components/wifi-manager/jquery.gz components/wifi-manager/code.js components/wifi-manager/index.html)
    register_component()
endif()
