#!/bin/bash
echo -e "\noption(BUILD_EXTRAKDESETTINGS \"Build the extension\" ON)\nif (BUILD_EXTRAKDESETTINGS)\n    add_subdirectory(Extra-KDE-Settings)\nendif()\n" >> ./CMakeLists.txt