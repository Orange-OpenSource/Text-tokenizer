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

#include <iostream>
#include <fstream>

#include "segmenter.h"
#include "unicode_tools.h"


using namespace std;


typedef enum { JSON, COL, LONG, SHORT } Output;

int main(int argc, char *argv[]) {

    Output output = COL;
    bool select = false;
    if (argc < 2) {
	cerr << "usage: " << argv[0] << " [options] segmentationrules [textfile]" << endl;
	cerr << "   options" << endl
	     << "   --out json|col|long|short   define output format" << endl
	     << "   --select                    output also segments which are  marked 'ignore'" <<endl
	     << endl;
	    
	return 1;
    }

    int shift = 1;

    for (int i = 1; i < argc - 1; ++i) {
	if (argv[i][0] != '-') break;
        if (!strcmp(argv[i], "--select")) {
	    shift++;
	    select = true;
	}
        else if (!strcmp(argv[i], "--out")) {
            shift += 2;
            if (!strcmp(argv[i + 1], "json")) output = JSON;
            else if (!strcmp(argv[i + 1], "col")) output = COL;
            else if (!strcmp(argv[i + 1], "long")) output = LONG;
            else if (!strcmp(argv[i + 1], "short")) output = SHORT;
            else {
                cerr << "invalid option value for --out: " << argv[i + 1] << endl;
                return 2;
            }
	    ++i;
	}
	else if (argv[i][0] == '-') {
            cerr << "invalid option: " << argv[i] << endl;
            return 3;
        }
    }

    string data = argv[shift];
    string textfile;
    if (shift+1 < argc) textfile = argv[shift+1];

    try {
        Segmenter *seg = new Segmenter(data);
	//if (out != "json" && !silent)  cerr << *seg << endl;

	if (textfile.empty()) {
	    // interactive

	    string TexteSource;
	    while(true) { // interactif
		cout << "?\n";
		getline(cin,TexteSource);
		
		if (cin.eof()) {	
		    break;// si fin de fichier on sort de ce while pour traiter le paragraphe en cours
		}
		if (cin.bad() || cin.fail()) {
		    cerr << "Error while reading from stdin" << endl;
		    return(12);
		}


		Text res;
		wchar_t *unicode = Unicode::fromUTF8(TexteSource.c_str());

		seg->segment(wstring(unicode), res, select);
		delete [] unicode;

		switch (output) {
		case JSON: res.json(cout); break;
		case COL: res.columns(cout); break;
		case LONG: res.longOutput(cout); break;
		case SHORT: cout << res; break;
		}
	    }
	} else {

	    ifstream ifp(textfile.c_str());
	    if (ifp) {
		string line;
		string input;
		while (true) {
		    char c;
		    c = ifp.get();
		    if (ifp.eof()) {
			break;
		    }
		    input += c;
		}

		Text res;
		wchar_t *unicode = Unicode::fromUTF8(input.c_str());
		seg->segment(wstring(unicode), res, select);
		delete [] unicode;

		switch (output) {
		case JSON: res.json(cout); break;
		case COL: res.columns(cout); break;
		case LONG: res.longOutput(cout); break;
		case SHORT: cout << res; break;
		}
		cout << endl;
	    }
	}

        delete seg;
    } catch (Segmenter::SegmenterException &e) {
        cerr << "ERRROR: " << e.what() << endl;
    }
}
