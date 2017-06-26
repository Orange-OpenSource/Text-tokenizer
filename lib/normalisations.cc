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


#include "normalisations.h"

#include <cstring>

/** Fonction supprimant des caracteres dans une chaine */

wchar_t *supprimeCaracteres(const wchar_t *chaine, const wchar_t *carASup) {
    wchar_t *tmp = new wchar_t[wcslen(chaine) + 1];
    wchar_t *nouveau_texte;
    unsigned int i;
    int k = 0;

    for (i = 0; i < wcslen(chaine); ++i) {	    
	int j;
	j = 0;
	while (carASup[j] && (chaine[i] != carASup[j])) {
	    j++;
	}
	if (!carASup[j]) {
	    tmp[k++] = chaine[i];
	}
    }
    tmp[k] = '\0';
    nouveau_texte = new wchar_t[k + 1];
    wcscpy(nouveau_texte, tmp);
    delete[] tmp;
    return nouveau_texte;
}


wchar_t *deleteSpaces(const wchar_t *chaine) {
    return supprimeCaracteres(chaine, L" ");
}
wchar_t *deletePoints(const wchar_t *chaine) {
    return supprimeCaracteres(chaine, L".");
}
