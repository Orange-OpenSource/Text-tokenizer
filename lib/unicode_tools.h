/** This library is under the 3-Clause BSD License

Copyright (c) 2017, Orange S.A.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors 
     may be used to endorse or promote products derived from this software without
     specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author: Johannes Heinecke
 Version:  1.0 as of 6th April 2017
*/

#ifndef __UNICODE_TOOL_H__
#define __UNICODE_TOOL_H__

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdint.h>

#include "UCD.h"


using std::ostream;
using std::string;
using std::wstring;
using std::vector;


typedef uint32_t Character;

/////////////////////////////////////////////////////////////////////////
/// @brief To manage string utilities
///
namespace OrangeLabsUtilities_Unicode {
    ostream & operator<<(ostream& out, const wstring& ws);

    class  Unicode {
    public:
	/** make a string printable (transformation of LF, TABS etc in escaped versions)
	@param s string
	@return result
	 */
	static string visibleString(const string& s); 

	/** split a string and put items into a vector, empty fields are deleted
	@param result 
	@param chaine string to split
	@param chaineRecherche separator string
	 */
	static void SplitNoEmptyFields(vector<string> &result,
			const string & chaine,
			const char * chaineRecherche);




	// the following functions are taken from
	// https://github.com/JeffBezanson/cutef8/blob/master/utf8.c
	// Basic UTF-8 manipulation routines  by Jeff Bezanson
	// placed in the public domain Fall 2005

	/** convert UTF-8 to UCS-4 (4-byte wide characters)
	    @param srcsz = source size in bytes, or -1 if 0-terminated
	    @param sz = dest size in # of wide characters
	    @returns # characters converted 
	*/
	static int u8_toucs(uint32_t *dest, int sz, const char *src, int srcsz);

	/** wrapper which does the correct allocation (to be freed by user) */
	static wchar_t *fromUTF8(const char *src);
	static char *toUTF8(const wchar_t *src);


        static const wstring getTextUppercase(const wstring &text);
        static const wstring getTextLowercase(const wstring &text);
        static const wstring getTextUnaccented(const wstring &text);


	/* srcsz = number of source characters, or -1 if 0-terminated
	   sz = size of dest buffer in bytes

	   returns # characters converted
	   dest will only be '\0'-terminated if there is enough space. this is
	   for consistency; imagine there are 2 bytes of space left, but the next
	   character requires 3 bytes. in this case we could NUL-terminate, but in
	   general we can't when there's insufficient space. therefore this function
	   only NUL-terminates if all the characters fit, and there's space for
	   the NUL as well.
	   the destination string will never be bigger than the source string.
	*/
	static int ws_to_utf8(char *dest, int sz, uint32_t *src, int srcsz);

	/* single character to UTF8 */
	static int wc_to_utf8(char *dest, uint32_t ch);



    };

} // end namespace

using namespace OrangeLabsUtilities_Unicode;

#endif
