#include "classificadorregressaologistica.h"
#include "../outros/auxfunc.h"

#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

#define PI 4*atan(1)

ClassificadorRegressaoLogistica::ClassificadorRegressaoLogistica(vector<string> clas, vector<string> atr, float* the):Classificador(){
    classes = clas;
    atributos = atr;
    theta = the;
}

bool ClassificadorRegressaoLogistica::executarClassificacao( Corpus &corpus, int atributo ){

int nExemplos, i, c, e, a, v, nAtributos, nConjExemplos;
    float total, val;
    string classe;

    nConjExemplos = corpus.pegarQtdConjExemplos();//numero de exemplos
    nAtributos = atributos.size();//numero de atributos + 1

    for (c=0; c < nConjExemplos; c++){
        nExemplos = corpus.pegarQtdExemplos(c);
        for (e=0; e < nExemplos; e++){
            total = 1.0*theta[0];

            for (a=0; a<nAtributos-1;a++){
                i = corpus.pegarPosAtributo(atributos[a]);
                v = corpus.pegarValor(c, e, i);
                (std::istringstream)(corpus.pegarSimbolo(v)) >> val >> std::dec;//converte para float
                total += theta[a + 1]*val;
            }

            classe = classes[(sigmoid(total) >= 0.5)?1:0];
            corpus.ajustarValor(c, e, atributo, corpus.pegarIndice(classe));
        }
    }

	return true;

}

string ClassificadorRegressaoLogistica::descricaoConhecimento(){
	ostringstream ret;
    return ret.str();
}

