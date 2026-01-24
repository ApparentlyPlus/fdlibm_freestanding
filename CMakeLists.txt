cmake_minimum_required(VERSION 3.10)
project(CustomMathSuite C)

set(CMAKE_C_STANDARD 99)
add_compile_definitions(_USE_MATH_DEFINES)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

if(MSVC)
    add_compile_options(/O2 /Oi /Ot)

    add_compile_options(/wd4244 /wd4459 /wd4146 /wd4389 /wd4018)
else()
    add_compile_options(-Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
endif()

set(MATH_RENAMES
    -Dsin=custom_sin     -Dcos=custom_cos     -Dtan=custom_tan
    -Dasin=custom_asin   -Dacos=custom_acos   -Datan=custom_atan
    -Datan2=custom_atan2
    -Dsinh=custom_sinh   -Dcosh=custom_cosh   -Dtanh=custom_tanh
    -Dasinh=custom_asinh -Dacosh=custom_acosh -Datanh=custom_atanh
    -Dexp=custom_exp     -Dlog=custom_log     -Dlog1p=custom_log1p
    -Dpow=custom_pow     -Dsqrt=custom_sqrt   -Dfabs=custom_fabs
    -Dceil=custom_ceil   -Dfloor=custom_floor -Dfmod=custom_fmod
    -Dtrunc=custom_trunc -Dround=custom_round
)

add_library(custom_math STATIC math.c)
target_compile_options(custom_math PRIVATE ${MATH_RENAMES})

if(MSVC)
    target_compile_options(custom_math PRIVATE /fp:strict)
endif()

add_executable(run_tests test_suite.c)
target_link_libraries(run_tests PRIVATE custom_math)

if(UNIX)
    target_link_libraries(run_tests PRIVATE m)
endif()

install(TARGETS run_tests DESTINATION bin)