cmake_minimum_required(VERSION 3.20)
project(ESP8266Client VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME ESP8266Wifi
        GITHUB_REPOSITORY ximtech/ESP8266Wifi
        GIT_TAG origin/main)

CPMAddPackage(
        NAME HTTPClient
        GITHUB_REPOSITORY ximtech/HTTPClient
        GIT_TAG origin/main)

set(ESP8266_CLIENT_DIRECTORY
        ${ESP8266_WIFI_DIRECTORY}
        ${ESP8266Client_SOURCE_DIR}
        ${ESP8266Client_SOURCE_DIR}/include
        CACHE STRING "ESP8266 client directories include to the main project" FORCE)

set(ESP8266_CLIENT_SOURCES
        ${ESP8266_WIFI_SOURCES}
        ${ESP8266Client_SOURCE_DIR}/include/ESP8266Client.h
        ${ESP8266Client_SOURCE_DIR}/ESP8266Client.c
        CACHE STRING "ESP8266 client source files include to the main project" FORCE)