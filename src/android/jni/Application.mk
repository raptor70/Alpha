# The ARMv7 is significanly faster due to the use of the hardware FPU
# APP_ABI := armeabi armeabi-v7a
APP_ABI := armeabi-v7a
APP_PLATFORM := android-11
# APP_STL := stlport_static
APP_STL := gnustl_static
APP_CPPFLAGS := -std=c++11 -frtti
APP_OPTIM := release