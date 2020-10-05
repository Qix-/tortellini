TEST_APP_NAME=test
BUILD_DIR=.
TEST_APP_SRC=$(wildcard tests/test.cc)
CATCH2_URL=https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp

.PHONY:${TEST_APP_NAME}

${TEST_APP_NAME}: build
	./${BUILD_DIR}/$@

clean:	
	rm ${BUILD_DIR}/${TEST_APP_NAME}

build:
	g++ --std=c++11 -o ${BUILD_DIR}/${TEST_APP_NAME} ${TEST_APP_SRC} -I.

