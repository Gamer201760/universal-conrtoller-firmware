# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include(/Users/gglamer/esp/v5.2/esp-idf/tools/cmake/project.cmake)
project(universal-controller)

target_add_binary_data(universal-controller.elf "main/mqtt_server.pem" TEXT)