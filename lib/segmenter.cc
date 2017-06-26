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

#include "segmenter.h"
#include "normalisations.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/regex.hpp>


#include "unicode_tools.h"




// TODO: resolve decomposition

using namespace std;
using namespace Segmenteur;



SegDef::SegDef(const string &re, const string &name, unsigned int id, bool ignore, bool endsentence, bool endpara, wchar_t *(*func)(const wchar_t *) = 0) :
    name(name),
    id(id),
    ignore(ignore),
    endSent(endsentence),
    endPara(endpara) {
    normalisation = func;
    wchar_t *dest = Unicode::fromUTF8(re.c_str());
    this->re = new boost::wregex((wchar_t *)dest, boost::wregex::optimize);
    delete [] dest;
}

SegDef::~SegDef() {
    delete re;
}

ostream& operator<<(ostream& out, const SegDef& s) {
    char *utf8 = Unicode::toUTF8(s.re->str().c_str());
    out << "\t" <<
	//s.re->str() << "\t" <<
            utf8 << "\t" <<
	s.name << "\t" <<
	s.ignore << "\t" <<
	s.endSent << "\t" <<
	s.endPara << "\t";
    delete [] utf8 ;
    return out;
}


Segmenter::Segmenter(const string &filename
#ifdef TILTMETHODS
		     , Recodeur *recodeur
#endif
		     ) 
#ifdef TILTMETHODS
: recodeur(recodeur)
#endif
{
    if (sizeof(wchar_t) != 4) {
        throw SegmenterException("Needs system where wchar_t has 4 bytes");
    }
    ifstream ifp(filename.c_str());

    //DecrypteurFichier decr(filename.c_str());
    //istream *ifp = decr.getFlux();

    if (ifp == 0) {
        throw new SegmenterException("cannot open segmentation rules " + filename);
    }

    string line;
    unsigned int ct = 0;
    bool mostGeneralRuleSeen = false;
    while(!ifp.eof()) {
        ct++;
	getline(ifp, line);
	if (line.empty()) continue;
	if (line[0] == '#') continue;
	//cerr << line << endl;

	vector<string>elems;
	Unicode::SplitNoEmptyFields(elems, line, "\t");
        if (elems[0].empty()) {
	    cerr << "Empty RegEx in segmenter rules in line " <<  ct << "; " << line << endl;
            continue;
        }
	bool ignore = false;
	bool es = false;
	bool ep = false;
	string normalisation_name;
	switch(elems.size()) {
	    case 6: normalisation_name = elems[5];
	    case 5: if (elems[4] == "yes") ep = true;
	    case 4: if (elems[3] == "yes") es = true;
	    case 3: if (elems[2] == "yes") ignore = true;
	    case 2: break;
	    default:
                ostringstream msg;
                msg << "invalid line (" << ct << "): " << line << endl;
                throw SegmenterException(msg.str());
                //continue;
	}

	wchar_t *(*normalisation)(const wchar_t *) = 0;
	
	if (!normalisation_name.empty()) {
	    if (normalisation_name == "DELETE_SPACES") {
		normalisation = deleteSpaces;
	    }
	    else if (normalisation_name == "DELETE_POINTS") {
		normalisation = deletePoints;
	    }
	    else {
                ostringstream msg;
                msg << "invalid normalisation (" << ct << "): " << line << endl;
                throw SegmenterException(msg.str());

	    }
	}

        if (elems[0] == ".") mostGeneralRuleSeen = true;
        try {
	    defs.push_back(new SegDef(elems[0], elems[1], defs.size(), ignore, es, ep, normalisation));
        } catch (boost::regex_error &e) {
            ostringstream msg;
            msg << "bad regular expresion (" << ct << "): " << line;
            throw SegmenterException(msg.str());
            //cerr << "bad regular expression (" << ct << "): " << line << endl;
        }
    }
    if (!mostGeneralRuleSeen) {
        defs.push_back(new SegDef(".", "ANY", defs.size(), false, false, false));
    }
}


Segmenter::~Segmenter() {
    for (vector<SegDef *>::iterator it = defs.begin(); it != defs.end(); ++it) {
	delete *it;
    }
}


ostream& operator<<(ostream& out, const Segmenter& s) {
    out << s.defs.size() << " segmentation rules" << endl;
    for (vector<SegDef *>::const_iterator it = s.defs.begin(); it != s.defs.end(); ++it) {
	out << **it << endl;
    }
    return out;
}

void Segmenter::segment(const wstring &text, Text &res, bool keepIgnored) {
    boost::match_flag_type flags = boost::match_continuous;
    std::wstring::const_iterator start = text.begin();
    std::wstring::const_iterator end = text.end();

    boost::match_results<std::wstring::const_iterator> what;

    Paragraph curPar;
    Sentence curSent;

    unsigned offset = 0;
    //    #define WITHLONGEST

    bool rtc;
    while (start < end) {
        bool segmentFound = false;
	
	vector<SegDef *>::iterator it;
#ifdef WITHLONGEST
	//if (getLongestMatch) {
	unsigned int maxlen = 0;
	it = defs.end();
	for(vector<SegDef *>::iterator its = defs.begin();  its != defs.end(); ++its) {
	    rtc = boost::regex_search(start, end, what, *(*its)->re, flags);

	    if (rtc) {
		wstring seg = what[0];
		if (seg.size() > maxlen) {
		    //cerr << "NEW SEG " << (*its)->name << " " 
		    // << maxlen << " --> " << seg.size() 
		    // << endl;
		    maxlen = seg.size();
		    it = its;
		}	       
	    }
	}
	
	if (it != defs.end() && maxlen != 0) {
	    rtc = true;
	} else {
	    rtc = false;
	}

#endif


#ifndef WITHLONGEST
	for(vector<SegDef *>::iterator it = defs.begin();
	    it != defs.end(); ++it) {

	    // cerr << "Test " << **it << endl;
#endif
	    rtc = boost::regex_search(start, end, what, *(*it)->re, flags);
	    if (rtc) {
		wstring seg = what[0];
		if (!seg.empty()) {
		    //cerr << "NAME " << (*it)->name 
		    //<< " '" << what[0] << "'" 
		    // << endl; 
		    start = what[0].second;

		    wstring orig = what[0];
		    unsigned int length = seg.size();
		    //	cerr << "lll: " << length << endl;
		    //char *utf8 = toUTF8(se)
		    if (keepIgnored || !(*it)->ignore) {
			//std::cerr <<  " " << offset << endl;
			if ((*it)->normalisation != 0) {
			    wchar_t *norm = ((*it)->normalisation)(orig.c_str());
			    seg = norm;
			    delete [] norm;

			    //cerr << "normalise: " << endl;
			    //wcerr << L"<" << orig << L"> ==> " 
			    //      << L"<" << seg << L">" << endl;
			}
		    
			curSent.push_back(Segment(seg, (*it)->name, (*it)->id,
						  offset, length,
						  (*it)->ignore,
						  orig
						  ));
#ifdef TILTMETHODS
			if (recodeur != 0) {
			    char *text8bit = recodeur->recoder(curSent.back().getUTF8Text().c_str());
			    if (strlen(text8bit) != 0) {
				curSent.back().add8BitText(text8bit);
			    }
			    free(text8bit);
			}
#endif
		    }
		    offset += length;

		    if ((*it)->endSent) {
			if (!curSent.empty()) curPar.push_back(curSent);
			curSent.clear();
		    }
		    if ((*it)->endPara) {
			if (!curSent.empty()) curPar.push_back(curSent);
			curSent.clear();
			if (!curPar.empty()) res.push_back(curPar);
			curPar.clear();
		    }
		    // rule matched
		    segmentFound = true;
#ifndef WITHLONGEST
		    break;
#endif
		}
	    }
#ifndef WITHLONGEST
	}
#endif
        if (!segmentFound || offset == 0) {
	    // TODO Exception
            cerr << "Jammed at " << offset << endl;
            break;
        }
	if (start >= end) {
	    break;
	}

    }
    if (!curSent.empty()) {
	curPar.push_back(curSent);
    }
    if (!curPar.empty()) {
        res.push_back(curPar);
    }
}

void Segmenter::listeDesTypesDeSegments(vector<string>& lts) const {
    lts.clear();
    for (vector<SegDef *>::const_iterator it = defs.begin(); it != defs.end(); ++it) {
	lts.push_back((*it)->name);
    }
}

void Segmenter::tableDesNumTypes(map<string,int>& tnt) const {
    tnt.clear();
    for (vector<SegDef *>::const_iterator it = defs.begin(); it != defs.end(); ++it) {
	    tnt[(*it)->name] = (*it)->id;
    }
}





