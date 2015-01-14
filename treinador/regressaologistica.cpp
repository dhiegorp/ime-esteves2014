#include "regressaologistica.h"

#include "../outros/auxfunc.h"

#include <string>
#include <sstream>
#include <iostream>

#include <math.h>
#include <stdlib.h>

RegressaoLogistica::RegressaoLogistica(vector<string> atr, vector<string> cla):Treinador(){
    classes = cla;
    atributos = atr;
}

float calculaCustoLog(float **X, int *y, float *theta, int m, int n){
    int i;
    float *HO, J;

    HO = new float[m];

    matrixVetMulti(X, theta, HO, m, n);
    inplace_sigmoid(HO, m);

    J = 0;
    for (i=0; i<m; i++)
        J -= y[i]*log(HO[i]) + (1 - y[i])*log(1.0 - HO[i]);
    J /= m;

    delete[] HO;
    return J;
    //
}

void gradientDescentLog(float **X, int *y, float *theta, int nExemplos, int nAtributos, float alpha = 0.01, int num_iters = 1500){
    int iter, i, j;
    float *HO, *sum, A;

    HO = new float[nExemplos];
    sum = new float[nAtributos];

    for (j=0; j<nAtributos;j++)
        theta[j]= 0.0;

    for (iter = 0; iter < num_iters; iter++){
        matrixVetMulti(X, theta, HO, nExemplos, nAtributos);
        inplace_sigmoid(HO, nExemplos);

        for (j=0; j<nAtributos;j++)
            sum[j] = 0.0;

        for (i = 0; i < nExemplos; i++){
            A = HO[i] - y[i];
            for (j=0; j<nAtributos;j++)
                sum[j] += A*X[i][j];
        }

        for (j=0; j<nAtributos;j++)
            theta[j] -= alpha*sum[j]/nExemplos;
    }

    delete[] HO;
    delete[] sum;
}

Classificador* RegressaoLogistica::executarTreinamento( Corpus &corpus, int atributo ){

    float **X, val, *theta;
    int *y, i, j, c, e, a, nExemplos, v, nAtributos, nConjExemplos, iClasse;
    map<int, int> mapaClasses;

    for (i=0; i<classes.size();i++){
        mapaClasses[corpus.pegarIndice(classes[i])] = i;
    }
    nAtributos = atributos.size();
    nConjExemplos = corpus.pegarQtdConjExemplos();

    theta = new float[nAtributos];
    X = new float*[nExemplos];
    y = new int[nExemplos];

    for (c=0; c < nConjExemplos; c++){
        nExemplos = corpus.pegarQtdExemplos(c);
        for (e=0; e < nExemplos; e++){
            X[e] = new float[nAtributos];
            //X[i][0] = 1.0;
            for (a=0; a<nAtributos;a++){
                i = corpus.pegarPosAtributo(atributos[a]);
                v = corpus.pegarValor(c, e, i);
                (std::istringstream)(corpus.pegarSimbolo(v)) >> val >> std::dec;//converte para float
                X[e][a] = val;
            }
            iClasse = mapaClasses[corpus.pegarValor(c, e, atributo)];
            y[e] = iClasse;
        }
    }

    gradientDescentLog(X, y, theta, nExemplos, nAtributos);

    delete[] y;
    for (i=0; i<nExemplos; i++){
        delete[] X[i];
    }
    delete[] X;

    return new ClassificadorRegressaoLogistica(classes, atributos, theta);

}
