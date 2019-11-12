cmake_minimum_required(VERSION 3.0)
project(SugarConvTape)


set(SRCS
	main.cpp
	)

set(HEADERS
	)

add_definitions( -D_CONSOLE)
add_executable(SugarConvTape ${SRCS} ${HEADERS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/../CPCCore/CPCCoreEmu)

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )


set ( CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} )

include_directories (SugarConvTape
					 )
set(CMAKE_CXX_STANDARD 17)


if (USE_SHARED_CRT)
else()
   foreach(CompilerFlag ${CompilerFlags})
     string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
   endforeach()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
	set ( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++1z )
endif()

MESSAGE ("ZLIB_LIBRARY : ${ZLIB_LIBRARY}")

if(MSVC)
	target_link_libraries ( SugarConvTape zlibstatic CPCCoreEmu)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")  
	ADD_DEFINITIONS(/arch:SSE2)
else ()
	target_link_libraries ( SugarConvTape  m stdc++fs CPCCoreEmu libz.a )
endif()
