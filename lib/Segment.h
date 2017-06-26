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


#ifndef SEGMENT_H
#define SEGMENT_H



#include <iostream>
#include <string>
#include <vector>
#ifdef TILTMETHODS
 #include <list>
 using std::list;
#endif

#include "unicode_tools.h"

using std::string;
using std::wstring;
using std::ostream;
using std::vector;

#ifdef TILTMETHODS
#include "SortieXML.h"
#endif

// seuil de pourcentage de majuscules pour que Phrase::toutEnMajuscule() est vrai
#define SEUIL_MAJ 0.9

// TODO: decide whether operator<<() or afficheTexte() display segmented sentence as text

namespace Segmenteur {

    class Segment {
    public:

        Segment(const wstring &s, const string & ty, unsigned id,
		unsigned o, unsigned l,
		bool i, const wstring &orig) :
	    text(s), type(ty), id(id),
	    offset(o), length(l), 
	    orig(orig),
	    ignore(i) {
            char *cutf8 = Unicode::toUTF8(text.c_str());
            
            utf8 = string(cutf8);
            delete [] cutf8;

	    if (orig != text) {
		normalised = true;
		cutf8 = Unicode::toUTF8(orig.c_str());
		orig_utf8 = string(cutf8);
		delete [] cutf8;
	    } else {
		normalised = false;
		orig_utf8 = utf8;
	    }
        };
                
        Segment() {    
        }
        
        ~Segment() {
        }

      
        
        bool isIgnored() const {
            return ignore;
        }

        ostream & json(ostream &out) const;
        ostream & columns(ostream &out) const;

        friend ostream& operator<<(ostream& out, const Segment& s);


#ifdef TILTMETHODS
        Segment *join(const Segment &other);
        
        /** rend vrai si le segment ne contient pas de minuscules (aussi vrai pour
            les segments contenant uniquement des chiffres etc. */
        bool sansMinuscules() const;

        /** rend vrai si le premier caractère est en majuscule et
            qu'au moins un est en minuscule */
        bool enFormeCapitalisee() const;

        /** rend vrai si le segment contient au moins une lettre */
        bool contientLettre() const;

        /** rend vrai si le segment ne contient que des chiffres */
        bool estUnNombre() const;
        
        /** si vrai, ce segment doit être sortie dans une balise XML spéciale */
	bool bypass;
        
      	/** rend oui, si ce segment doit être sortie dans une balise XML spéciale */
	bool getBypass() const { return bypass; };
	void setBypass(bool b) { bypass = b; };  

	TILTXMLNODE sortieEnXML(SortieXML *sx, unsigned int format) const;

	
#endif

	//#define texte_brut getUTF8Text
	/** returns the orginal, unnormalised text */
        const string & texte_brut() const {
            return orig_utf8;
        }

	/** returns the normalised text */
        const string & getUTF8Text() const {
            return utf8;
        }


	/** long and verbose output */
	ostream & longOutput(ostream& out) const;

	/** compatibility output with old segmenter */
	ostream & compatOutput(ostream &out) const;


	/** returns just the normalised text */
        const string & shortOutput() const {
            return utf8;
        }

        const wstring & getText() const {
            return text;
        }

        const wstring & getOrig() const {
            return orig;
        }
        
        const string &getType() const {
            return type;
        }
        unsigned int getNumtype() const {
            return id;
        }
        
        unsigned int getOffset() const { return offset; }
        unsigned int getLength() const { return length; }

#ifdef TILTMETHODS
	const string & get8BitText() const {
	    return text8bit;
	}


	void add8BitText(const string &t) {
	    text8bit = t;
	}
#endif

    private:
        wstring text; /// unicode
        string utf8;
#ifdef TILTMETHODS
        string text8bit; /// iso8859-1 or other 8bit coding. Can be deleted, when lexicon is Unicode
#endif
        string type; /// type du segment (nom)
        unsigned id; /// type du segment (forme num�rique)
        unsigned offset;
        unsigned length;
        wstring orig; /// unicode
	string orig_utf8;
        bool ignore;
	bool normalised;

    };

    class Sentence : public vector<Segment> {
    public:
        friend ostream& operator<<(ostream& out, const Sentence& s);
        ostream & json(ostream &out) const;
        ostream & columns(ostream &out) const;
	ostream & longOutput(ostream& out) const;

#ifdef TILTMETHODS
	/** compatibility output with old segmenter */
	ostream & compatOutput(ostream &out) const;

        bool sansAccents() const;
        bool toutEnMajuscules() const;

	void affichePhrase(ostream &out) const;
	TILTXMLNODE sortieEnXML(SortieXML *sx) const;
#endif
    };

    class Paragraph : public vector<Sentence> {
    public:
        friend ostream& operator<<(ostream& out, const Paragraph& p);
	ostream & longOutput(ostream& out) const ;
        ostream & json(ostream &out) const;
        ostream & columns(ostream &out) const;

#ifdef TILTMETHODS
	/** compatibility output with old segmenter */
	ostream & compatOutput(ostream &out) const;

	TILTXMLNODE sortieEnXML(SortieXML *sx) const;
#endif
    };

    class Text : public vector<Paragraph> {
    public:
        friend ostream& operator<<(ostream& out, const Text& t);
	ostream & longOutput(ostream& out) const ;
        ostream & json(ostream &out) const;
        ostream & columns(ostream &out) const;

#ifdef TILTMETHODS
	/** compatibility output with old segmenter */
	ostream & compatOutput(ostream &out) const;

	void afficheTexte(list<int> numerosPhrase, ostream& out,bool decalage, bool allSentences=false);
	unsigned int getNumberOfParagraphs();
	unsigned int getNumberOfPhrases();
	TILTXMLNODE sortieEnXML(SortieXML *sx) const;
#endif
    };
}
#endif /* SEGMENT_H */

