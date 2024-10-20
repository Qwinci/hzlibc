target_sources(c PRIVATE
	setjmp.cpp
	../float_gcc.cpp
)

add_library(crti OBJECT crti.S)
add_library(crtn OBJECT crtn.S)
add_library(crt1 OBJECT crt1.S)
add_library(Scrt1 OBJECT scrt1.S)

install(
	FILES $<TARGET_OBJECTS:crti> RENAME crti.o
	TYPE LIB
)

install(
	FILES $<TARGET_OBJECTS:crtn> RENAME crtn.o
	TYPE LIB
)

install(
	FILES $<TARGET_OBJECTS:crt1> RENAME crt1.o
	TYPE LIB
)

install(
	FILES $<TARGET_OBJECTS:Scrt1> RENAME Scrt1.o
	TYPE LIB
)
