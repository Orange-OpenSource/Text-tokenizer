#!/usr/bin/python

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

# makes a c++ library from UCD's UnicodeData.txt
# unicode data downladed at http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
import sys
import string



# if we need unicode points > FFFF uint16_t must changed do uint32_t
MAXCODEPOINT=0xffff

header = """// automatically created by ucd2cc.py

#ifndef __UCD_H__
#define __UCD_H__

#include <stdint.h>

typedef enum {%s} TypeCode;

struct UCD {
    uint%d_t codepoint;
    // const char *name;
    // ...
    //const char *type;
    TypeCode typecode;
    uint%d_t other_case;
    unsigned short whichcase; // 0: any, 1: lower; 2: upper
    //bool canBeComposed;
    uint%d_t unaccented; // only usefull for characters composed of two symbols
    uint%d_t accent;
    bool anyLetter; // any of Lu Lt Ll, Lo (but not Lm)
};

extern struct UCD unicodedata [];

#endif
"""



class UCD2CC:
	def __init__(self, ucdfile, outbasename):
		ofp = open("%s.cc" % outbasename, "w")
		ofp.write('#include "%s.h"\n' % outbasename)
		ofp.write('struct UCD unicodedata[] = {')

		typenum = set()
		typenum.add("None")
		#typenum.add("Lo")
		#typenum.add("Lt")
		#typenum.add("Lu")
		#typenum.add("Ll")

		ifp = open(ucdfile)
		cct = 0
		for line in ifp.readlines():
			line = line.strip()
			elems = line.split(";")
			numval = int(elems[0], 16)
			if numval > MAXCODEPOINT: 
				# for the time being we do not need more
				break
			#print cct, numval
			while (numval > cct):
				ofp.write('\t{0x%x, None, 0, 0, false, 0,0 }, // missing\n' % (cct))
		
				cct += 1
			#print "zzz %x %x" %(cct, numval)
			othercase = '0';
			case = 0;
			if elems[2] == "Lu" and elems[13]:
				case = 2
				othercase = "0x%s" % elems[13];
			elif elems[2] == "Ll" and elems[12]:
				case = 1
				othercase = "0x%s" % elems[12];
			typenum.add(elems[2])
			unaccented = "0"
			accent = "0"
			if elems[5]: 
				components = elems[5].strip().split()
				if len(components) == 2 and components[0][0] != "<":
					unaccented = "0x%s" % components[0]
					accent = "0x%s" % components[1]
			anyLetter = "false"
			if elems[2][0] == "L" and elems[2][1] != "m": anyLetter = "true"
			ofp.write('\t{0x%x, %s, %s, %d, %s, %s, %s}, // %s\n' \
				% (numval,
				 elems[2],
				 othercase,
				 case,
				 unaccented,
				 accent,
				 anyLetter,
				 elems[1]))
			cct += 1

		ofp.write('\t{0, None, 0, 0, false, 0,0}\n};\n')
		ofp.close()


		inttype = 16
		if MAXCODEPOINT >0xffff:
			inttype = 32
		ofph = open("%s.h" % outbasename, "w")
		ofph.write(header % (string.join(typenum, ", "), inttype, inttype, inttype, inttype))

		ofph.close()

if __name__ == "__main__":

	if len(sys.argv) < 3:
		sys.stderr.write("usage: %s in:unicodedata.txt out:basename\n" % sys.argv[0])
		sys.exit(1)
	else:
		UC = UCD2CC(sys.argv[1], sys.argv[2])


