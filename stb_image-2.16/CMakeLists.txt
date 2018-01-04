cmake_minimum_required(VERSION 2.8)

set(STBIMAGE_PUBLIC_HEADERS stb_image.h)

install(FILES ${STBIMAGE_PUBLIC_HEADERS} DESTINATION include/stb_image)
