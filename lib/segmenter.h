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

#ifndef SEGMENTER_H
#define SEGMENTER_H


#include <iostream>
#include <string>
#include <boost/regex.hpp>

#include "Segment.h"

#ifdef TILTMETHODS
 #include "TiltException.h"
 #include "codeurUTF.h"
#endif


using std::ostream;
using std::string;
using std::wstring;
using std::endl;
using std::map;


using namespace Segmenteur;
/** definition of a segmenter rule */
class SegDef {
 public:
    SegDef(const string &re, const string &name, unsigned int id, bool ignore, bool endsentence, bool endpara, wchar_t *(*func)(const wchar_t *) );
    ~SegDef();

    friend ostream& operator<<(ostream& out, const SegDef& s);

    boost::wregex *re;
    string name;
    unsigned int id;
    bool ignore;
    bool endSent;
    bool endPara;
    wchar_t *(*normalisation)(const wchar_t *);
};


/** text segmentation class */
class Segmenter {
 public:
    /**
       @param filename file with segmentation rules
       @param recodeur coding tool to produce 8bit version of segments (to be able to use Analex)
     */
    Segmenter(const string &filename
#ifdef TILTMETHODS
	      , Recodeur *recodeur = 0
#endif
	      );
    ~Segmenter();

    void segment(const wstring &text, Text &result, bool keepIgnored=false);
    friend ostream& operator<<(ostream& out, const Segmenter& s);

    void listeDesTypesDeSegments(vector<string>& lts) const;
    void tableDesNumTypes(map<string, int>& tnt) const;

    class SegmenterException {
    public:
        SegmenterException(const string &msg) {
            this->message = msg;
        }

	string what() const {
	    return message;
	}
    private:
	string message;
    };

 private:
    vector<SegDef *>defs;
#ifdef TILTMETHODS
    Recodeur *recodeur;
#endif
};


#endif
