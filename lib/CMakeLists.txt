# This library is under the 3-Clause BSD License
# 
# Copyright (c) 2017, Orange S.A.
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
# 
#   2. Redistributions in binary form must reproduce the above copyright notice, 
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
# 
#   3. Neither the name of the copyright holder nor the names of its contributors 
#      may be used to endorse or promote products derived from this software without
#      specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#  Author: Johannes Heinecke
#  Version:  1.0 as of 6th April 2017


set(UCDATA_URL "http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt")
#set(UCDATA_TXT "${CMAKE_CURRENT_BINARY_DIR}/UnicodeData.txt")
set(UCDATA_TXT "${PROJECT_SOURCE_DIR}/downloaded_data/UnicodeData.txt")
set(UCDCC_DIR "${CMAKE_CURRENT_BINARY_DIR}")


add_library(libsegment
	    normalisations.cc
	    Segment.cc
	    segmenter.cc
	    ${UCDCC_DIR}/UCD.cc
	    unicode_tools.cc)


set(HEADERS
	normalisations.h
	segmenter.h
	Segment.h
	unicode_tools.h
	${UCDCC_DIR}/UCD.h
)

# download information on Unicode data
if (NOT EXISTS "${UCDATA_TXT}")
    file(DOWNLOAD "${UCDATA_URL}" "${UCDATA_TXT}" STATUS status)
    message(STATUS "Unicode data download: ${status}")

    list(GET status 0 status_code)
    # list(GET status 1 status_string)


    if(NOT status_code EQUAL 0)
	message(FATAL_ERROR "error: downloading '${UCDATA_URL}' failed")
    endif()
endif()

# wrap Unicode data into a c++ class
add_custom_command(
    OUTPUT "${UCDCC_DIR}/UCD.cc"
    COMMAND ${PROJECT_SOURCE_DIR}/lib/ucd2cc.py ${UCDATA_TXT} ${CMAKE_CURRENT_BINARY_DIR}/UCD
    DEPENDS "${UCDATATXT}")

# UCD.h is written to ${CMAKE_CURRENT_SOURCE_DIR}, so we add this to the include_dirs
target_include_directories (libsegment PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}
			${CMAKE_CURRENT_BINARY_DIR}
	)


install (TARGETS libsegment DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include/segmenter)