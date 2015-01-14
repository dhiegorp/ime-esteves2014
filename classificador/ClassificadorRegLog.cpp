#include "ClassificadorRegLog.h"

#include "../outros/auxfunc.h"
#include "../corpus/corpus.h"
#include "../corpus/corpusmatriz.h"
#include "../treinador/treinador.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <math.h>
#include <stdlib.h>

//bool ClassificadorRegLog::executarClassificacao(Corpus &corpus, int atributo)
//{
//    int qtdExemplos,nConjExemplos, nExemplos, c,e, j, v, n;
//
//    unsigned int iatr;
//    float total, val;
//    string c2;
//
//    //qtdExemplos = corpus.pegarQtdExemplos(0);//numero de exemplos
//    n = theta.size()-1;//numero de atributos + 1
//
//    nConjExemplos = corpus.pegarQtdConjExemplos();
//    for (c=0; c < nConjExemplos; c++){
//        nExemplos = corpus.pegarQtdExemplos(c);
//        for (e = 0; e < nExemplos; e++){
//
//            total = theta[0];
//            for (j=0; j < n-1; j++)
//            {
//                iatr = corpus.pegarPosAtributo(atributos[j]);
//                v = corpus.pegarValor(c, e, iatr);
//                (std::istringstream)(corpus.pegarSimbolo(v)) >> val >> std::dec;//converte para float
//                total += theta[j + 1] * val;
//            }
//
//            float e2 = exp(total * -1.0);
//            float sig = 1/(1+e2);
//            int cl;
//            if ((sig > 0.45) && (sig <=1.0))
//            {
//                cl =1; //-1
//            }
//            else if ((sig>=0.0) && (sig<=0.45))
//            {
//                cl =0; //+1
//            }
//            else
//            {
//                throw "erro na função logit. funcao sigmoid fora do intervalo";
//            }
//
//            //  cout << "cl = " << cl;
//            c2 = classes[cl];
//            corpus.ajustarValor(c, e, atributo, corpus.pegarIndice(c2));
//
//        }
//    }
//
//    return true;
//}

//void ClassificadorRegLog::ajustarTheta2(vector<float> theta, int n)
void ClassificadorRegLog::ajustarTheta2(float *theta, int n)
{
    //this->theta.resize(n);
    //for (int i=0; i<n; i++)
    //    this->theta[i] = theta[i];

     this->theta.resize(n);
        for (int i = 0; i < n; i++)
            this->theta[i] = theta[i];

}

void ClassificadorRegLog::ajustarAtributos(vector<string> atributos, int n)
{
    this->atributos.resize(n);
    for (int i=0; i<n; i++)
        this->atributos[i] = atributos[i];
}

void ClassificadorRegLog::ajustarClasses(vector<string> classes)
{
    this->classes = classes;
}


ClassificadorRegLog::ClassificadorRegLog(vector<string> classes)
{
    this->classes = classes;
    //ctor
}

ClassificadorRegLog::~ClassificadorRegLog()
{
    theta.resize(0);
    //dtor
}
