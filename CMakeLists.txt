cmake_minimum_required(VERSION 3.15)
project(ESPName CXX)

set(CMAKE_CXX_STANDARD 17)

# สร้างไฟล์ผลลัพธ์เป็น .asi
add_library(ESPName SHARED main.cpp)
set_target_properties(ESPName PROPERTIES SUFFIX ".asi")

# ลิงก์ไลบรารี psapi ของ Windows
target_link_libraries(ESPName PRIVATE psapi)
