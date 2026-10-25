cmake_minimum_required(VERSION 3.15)
project(ESPName ASI CXX)

set(CMAKE_CXX_STANDARD 17)

# บิวด์ main.cpp ออกมาเป็นไฟล์ .asi
add_library(ESPName SHARED main.cpp)
set_target_properties(ESPName PROPERTIES SUFFIX ".asi")

# ลิงก์ไลบรารี psapi สำหรับ Windows API
target_link_libraries(ESPName PRIVATE psapi)
