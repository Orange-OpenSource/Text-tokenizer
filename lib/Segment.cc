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

#include "Segment.h"
#include "UCD.h"
#include "unicode_tools.h"
#include <cstring>
#include <sstream>


// TODO: accès lexique utilisateur en UTF8

using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;
using std::endl;
using std::cout;
namespace Segmenteur {

    ostream& operator<<(ostream& out, const Segment& s) {
	if (s.normalised) {
	    out << "\"" << Unicode::visibleString(s.texte_brut()) << "\" : ";
	}

        out << '"' << Unicode::visibleString(s.getUTF8Text()) << "\" : " << s.type
	    << "," << s.offset << ","  << (s.offset + s.length)
	    ;
        return out;
    }

    ostream & Segment::longOutput(ostream& out) const {
        char *uppercase = Unicode::toUTF8(Unicode::getTextUppercase(text).c_str());
        char *lowercase = Unicode::toUTF8(Unicode::getTextLowercase(text).c_str());
        char *unaccented = Unicode::toUTF8(Unicode::getTextUnaccented(text).c_str());
        out << "   \"" << Unicode::visibleString(utf8);

	if (normalised) {
	    out << "\" < \"" << Unicode::visibleString(orig_utf8);
	}
	out << "\"\t(" << type 
	    << "," << offset << "," << (offset + length) << ")"
	    << "\t\"" << Unicode::visibleString(uppercase) << "\"\t\"" 
	    << Unicode::visibleString(lowercase) << "\"\t\""
	    << Unicode::visibleString(unaccented)
#ifdef TILTMETHODS
	    << "\"\t" << (sansMinuscules() ? " no lowercase;" : "")
	    << (enFormeCapitalisee() ? " capitalised;" : "")
	    << (contientLettre() ? " contains letter;" : "")
	    << (estUnNombre() ? " is number;" : "")
#endif
	    << endl;
        delete [] uppercase;
        delete [] lowercase;
        delete [] unaccented;
        return out;
    }


    ostream & Segment::compatOutput(ostream& out) const {
	string txt = Unicode::visibleString(utf8);
	string txt_brut = Unicode::visibleString(orig_utf8);
	out << "    [";
	if(normalised)
	    out<<  txt_brut << " => ";
	out<<  txt << "]"
	   << (text.size() < 2 ? "\t\t\t" : (text.size() < 10 ? "\t\t" : "\t"))
	   << type << endl;
	return out;
    }

    ostream & Segment::json(ostream &out) const {
        //char *utf8 = toUTF8(text.c_str());
        out << '{'
	    << "\"segment\":\"" << Unicode::visibleString(getUTF8Text()) << '"'
	    << ",\"typeid\":\"" << id << '"'
	    << ",\"type\":\"" << type << '"';
	   
	if (normalised) {
	    out << ",\"original\":\"" << Unicode::visibleString(orig_utf8) << '"';
	}
	out << ",\"offset\":" << offset
	    << ",\"length\":" << length;
	out << '}';
        return out;
    }

    ostream & Segment::columns(ostream &out) const {
        //char *utf8 = toUTF8(text.c_str());
        out << Unicode::visibleString(getUTF8Text()) << "\t" << type << endl;
        return out;
    }

#ifdef TILTMETHODS
    Segment *Segment::join(const Segment &other) {
        wstring concatenated = text + other.getText();
	wstring orig_concatenated = orig + other.getOrig();
        Segment *newseg = new Segment(concatenated,
				      other.getType(), other.getNumtype(),
				      offset, length+other.getLength(),
				      false,
				      orig_concatenated);
	string _8bittext = text8bit + other.get8BitText();
	newseg->add8BitText(_8bittext);
        return newseg;
        
    }
    
    
    bool Segment::sansMinuscules() const {
        unsigned int n = 0;
        for (wstring::const_iterator it = text.begin(); it != text.end(); ++it) {
	    //cerr << "cc " << (unsigned int) *it << endl;
            if (unicodedata[*it].typecode == Ll) n += 1;
	    else if (unicodedata[*it].typecode == Lo) n += 1; // si les caractères des alphabets sans casse sans présents, on augment aussi
        }
        return n == 0;
    }

    bool Segment::contientLettre() const {
	//std::cerr << "CONTIENT LETTRE? " << *this << endl;
        for (wstring::const_iterator it = text.begin(); it != text.end(); ++it) {
            if (unicodedata[*it].anyLetter) { 
		//fprintf(stderr, "%x ", (unsigned int)*it);
		//std::cerr << "yes ." << (unsigned int)*it << "." << endl;
		return true;
	    }
        }
        return false;
    }

    bool Segment::estUnNombre() const {
        for (wstring::const_iterator it = text.begin(); it != text.end(); ++it) {
            if (unicodedata[*it].typecode != Nd) return false;
        }
        return true;
    }

    bool Segment::enFormeCapitalisee() const {
        if (unicodedata[text[0]].typecode != Lu) return false;
	return true;
	/*
        for (wstring::const_iterator it = text.begin(); it != text.end(); ++it) {
            if (unicodedata[*it].typecode == Ll) return true;
        }
        return false;
	*/
    }

    TILTXMLNODE Segment::sortieEnXML(SortieXML *sx, unsigned int format) const {
	TILTXMLNODE noeudARendre = sx->creerNoeud("SEGMENT");
	sx->ajouterAttribut(noeudARendre, "TYP", getType().c_str());
	//MODIF 03/04/2009 (SLM) :  ajouter la forme brute si pas vide
	//if (!texte_brut().empty()) {
	//    sx->ajouterAttributUTF8(noeudARendre, "ORIG", utf8.c_str());
	//}
	sx->ajouterAttributUTF8(noeudARendre, "VAL", utf8.c_str());
	sx->ajouterAttribut(noeudARendre, "OFFSET", offset);    
	sx->ajouterAttribut(noeudARendre, "ORIGSIZE", length);    
    // ajouter la forme brute si pas vide
    //if (!texte_brut().empty()) {
    //	sx->ajouterAttributLangueSource(noeudARendre, "ORIG", texte_brut().c_str());
    //}

	return noeudARendre;
}


#endif


    // ================== Sentence =================

    ostream& operator<<(ostream& out, const Sentence& s) {
        for (Sentence::const_iterator segit = s.begin(); segit != s.end(); ++segit) {
	    //if (segit != s.begin()) out << " ";
            //out << segit->getUTF8Text();
	    out << *segit << endl;
        }
	return out;
    }

    ostream & Sentence::longOutput(ostream& out) const {
        out << "  SENTENCE"
#ifdef TILTMETHODS
                << (sansAccents() ? " no accents;" : "")
                << (toutEnMajuscules() ? " all uppercase;" : "")
#endif
                << endl;
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            //if (!it->isIgnored())
            out << "    ";
	    segit->longOutput(out);
        }
        out << "  ENDSENTENCE" << endl;
        return out;
    }


#ifdef TILTMETHODS
    ostream & Sentence::compatOutput(ostream& out) const {
        out << "  DEBUT PHRASE(taille " << size() << ")" << endl;
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
	    segit->compatOutput(out);
        }
        out << "  FIN PHRASE" << endl;
        return out;
    }
#endif

    ostream & Sentence::json(ostream &out) const {
        out << "{\"segments\":[";
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            if (segit != begin()) out << ',';
            segit->json(out);
        }
        out << "]}";
        return out;
    }

    ostream & Sentence::columns(ostream &out) const {
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            segit->columns(out);
        }
        out << endl;
        return out;
    }

#ifdef TILTMETHODS
    void Sentence::affichePhrase(ostream &out) const {
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            //if (!it->isIgnored())
	    //if (segit != begin()) out << " ";
            out << segit->getUTF8Text() << " ";
        }

    }
    
    bool Sentence::sansAccents() const {
        int n = 0;
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            for (wstring::const_iterator it = segit->getText().begin(); it != segit->getText().end(); ++it) {
                if (unicodedata[*it].accent != 0 && unicodedata[*it].anyLetter) n++;
            }
        }
        return n == 0;
    }

    bool Sentence::toutEnMajuscules() const {
        unsigned int nMaj = 0;
        unsigned int nMin = 0;
        for (Sentence::const_iterator segit = begin(); segit != end(); ++segit) {
            for (wstring::const_iterator it = segit->getText().begin(); it != segit->getText().end(); ++it) {
                if (unicodedata[*it].typecode == Ll) nMin++;
                else if (unicodedata[*it].typecode == Lu) nMaj++;
            }
        }

        if (nMin == 0 && nMaj != 0) return true;
        float seuil = ((nMaj * 1.0) / ((nMaj + nMin) * 1.0));
        if (seuil < SEUIL_MAJ) {
            return false;
        } else
            return true;
    }


    TILTXMLNODE Sentence::sortieEnXML(SortieXML *sx) const {
	TILTXMLNODE noeudARendre = sx->creerNoeud("PHRASE");
#ifdef __TILT_LOG_ON__
	ostringstream msg;
	msg << size();
	sx->ajouterAttribut(noeudARendre, "LEN", msg.str().c_str());
#endif
	for (Sentence::const_iterator seg = begin(); 
	     seg != end(); ++seg) {
	    sx->ajouterNoeud(noeudARendre, seg->sortieEnXML(sx, 0));
	}
	return noeudARendre;
    }



#endif


    // ================== Paragraph =================

    ostream & Paragraph::longOutput(ostream& out) const {
        out << " PARAGRAPH" << endl;
        for (Paragraph::const_iterator sentit = begin(); sentit != end(); ++sentit) {
            sentit->longOutput(out);
	    out << endl;
        }
        out << " ENDPARAGRAPH" << endl;
        return out;
    }

#ifdef TILTMETHODS
    ostream & Paragraph::compatOutput(ostream& out) const {
        out << "DEBUT PARAGRAPH" << endl;
        for (Paragraph::const_iterator sentit = begin(); sentit != end(); ++sentit) {
            sentit->compatOutput(out);
        }
        out << "FIN PARAGRAPH" << endl;
        return out;
    }
#endif

    ostream& operator<<(ostream& out, const Paragraph& p) {
        for (Paragraph::const_iterator sentit = p.begin(); sentit != p.end(); ++sentit) {
            out << *sentit << endl;
        }
        return out;
    }

    ostream & Paragraph::json(ostream &out) const {
        out << "{\"sentences\":[";
        for (Paragraph::const_iterator sentit = begin(); sentit != end(); ++sentit) {
            if (sentit != begin()) out << ',';
            sentit->json(out);
        }
        out << "]}";
        return out;
    }

    ostream & Paragraph::columns(ostream &out) const {
        for (Paragraph::const_iterator sentit = begin(); sentit != end(); ++sentit) {
            sentit->columns(out);
        }
        return out;
    }

#ifdef TILTMETHODS
    TILTXMLNODE Paragraph::sortieEnXML(SortieXML *sx) const {
	TILTXMLNODE noeudARendre = sx->creerNoeud("PARAGRAPHE");
#ifdef __TILT_LOG_ON__
	ostringstream msg;
	msg << size();
	sx->ajouterAttribut(noeudARendre, "LEN", msg.str().c_str());
#endif

	for (vector<Sentence>::const_iterator ph = begin(); 
	     ph != end(); ++ph) {
	    sx->ajouterNoeud(noeudARendre, ph->sortieEnXML(sx));
	}
	return noeudARendre;
    }
#endif



    // ================== Text =================

    ostream& operator<<(ostream& out, const Text& t) {
        for (Text::const_iterator parit = t.begin(); parit != t.end(); ++parit) {
	    //out <<"TEXT" << endl;
            out << *parit;
	    //out <<"ENDTEXT" << endl;
        }
        return out;
    }

    ostream & Text::longOutput(ostream &out) const {
        for (Text::const_iterator parit = begin(); parit != end(); ++parit) {
	    out <<"TEXT" << endl;
            parit->longOutput(out);
	    out <<"ENDTEXT" << endl;
        }
        return out;
    }

#ifdef TILTMETHODS
    ostream & Text::compatOutput(ostream &out) const {
        for (Text::const_iterator parit = begin(); parit != end(); ++parit) {
            parit->compatOutput(out);
        }
        return out;

    }
#endif

    ostream & Text::json(ostream &out) const {
        out << "{\"paragraphs\":[";
        for (Text::const_iterator parit = begin(); parit != end(); ++parit) {
            if (parit != begin()) out << ',';
            parit->json(out);
        }
        out << "]}";
        return out;
    }

    ostream & Text::columns(ostream &out) const {
        for (Text::const_iterator parit = begin(); parit != end(); ++parit) {
            parit->columns(out);
        }
        return out;
    }

#ifdef TILTMETHODS
    void Text::afficheTexte(list<int> numerosPhrase, ostream& out,bool decalage,bool allSentences) {
	int numeroPhrase=0;
	if (decalage) numeroPhrase++;
	for (vector<Paragraph>::const_iterator p = begin(); p != end(); ++p) {
	    for (vector<Sentence>::const_iterator ph = p->begin(); ph != p->end(); ++ph) {
		for (list<int>::const_iterator i= numerosPhrase.begin();
		     i != numerosPhrase.end(); ++i) {
		    if (*i==numeroPhrase||allSentences) ph->affichePhrase(out);
		    // if (*i==numeroPhrase) {out << "Phrase " ; } ;
		}
		//  out  << numeroPhrase;ph->affichePhrase(out);
		numeroPhrase++;
	    }
	}
    }

    unsigned int Text::getNumberOfParagraphs() {
	return this->size();
    }

    unsigned int Text::getNumberOfPhrases() {
	unsigned int total = 0;
	for (vector<Paragraph>::const_iterator p = begin(); p != end(); ++p) {
	    for (vector<Sentence>::const_iterator s = p->begin(); s != p->end(); ++s) {
		// on ne prend que les phrases non-vides
		if (!s->empty()) total++;
	    }
	}
	return total;
    }

    TILTXMLNODE Text::sortieEnXML(SortieXML *sx) const {
	TILTXMLNODE noeudARendre = sx->creerNoeud("TEXTE");
	for (vector<Paragraph>::const_iterator p = begin(); 
	     p != end(); ++p) {
	    sx->ajouterNoeud(noeudARendre, p->sortieEnXML(sx));
	}
	return noeudARendre;
    }


#endif


}

