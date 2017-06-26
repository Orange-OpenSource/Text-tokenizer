# This library is under the Modified BSD License:
# 
# Copyright (c) 2017, Orange S.A. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
#   * Redistributions of source code must retain the above copyright notice, 
#     this list of conditions and the following disclaimer.
# 
#   * Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
# 
#   * Neither the name of the Orange S.A. nor the names of its contributors may be used
#     to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL ORANGE S.A. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


add_executable (textSegmenter textSegmenter.cc)



if (${CMAKE_MAJOR_VERSION} VERSION_LESS 3.0)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
else()
	set_property(TARGET textSegmenter PROPERTY CXX_STANDARD 11)
        #set_property(GLOBAL PROPERTY CXX_STANDARD 11)
        #set_property(GLOBAL PROPERTY C_STANDARD 11)
endif()



target_link_libraries (textSegmenter LINK_PUBLIC
			 libsegment
			 ${Boost_LIBRARIES})




add_custom_target(mini_test
	COMMAND ./textSegmenter --out long ${PROJECT_SOURCE_DIR}/example/seg.data ${PROJECT_SOURCE_DIR}/example/test.txt | tee result.txt
	COMMAND echo comparing: result.txt ${PROJECT_SOURCE_DIR}/example/result_reference.txt
	COMMAND meld result.txt ${PROJECT_SOURCE_DIR}/example/result_reference.txt
)

add_custom_target(valgrind_test
	COMMAND cat ${WORDS} | valgrind --log-file=${CMAKE_CURRENT_BINARY_DIR}/val.log
				--track-origins=yes -v --num-callers=25 
				--tool=memcheck
				--leak-check=full 
				--leak-resolution=high
				--show-reachable=yes
				./textSegmenter --out long ${PROJECT_SOURCE_DIR}/example/seg.data ${PROJECT_SOURCE_DIR}/example/test.txt
	COMMAND echo "valgrind log: ${CMAKE_CURRENT_BINARY_DIR}/val.log"
		)

add_dependencies(mini_test textSegmenter)

