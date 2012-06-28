include_directories(../Includes)
include_directories(../Wrappers)
include_directories(../Act)
add_library(
  Act
  ACT_Sub_DLL.cpp
  INJECTION_RATE.cpp)
