# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.1.4/components/bootloader/subproject"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/tmp"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/src/bootloader-stamp"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/src"
  "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/vs-idf/v5.1-project/Smart_ security/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
