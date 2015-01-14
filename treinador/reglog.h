#ifndef REGLOG_H
#define REGLOG_H

#include <map>
#include <vector>
#include "treinador.h"
#include "../outros/auxfunc.h"
#include "../corpus/corpus.h"
#include "../corpus/corpusmatriz.h"


class RegLog : public Treinador
{
    vector<string> classes, atributos;
    float const_e, alpha, lamb, Jmin;
    int iter;
public:
    RegLog(vector<string> classes, int iter, float alpha, float lamb, float Jmin, float const_e);
    virtual ~RegLog();
    virtual Classificador *executarTreinamento(Corpus &corpus, int atributo);
    void ajustarParametros(vector<string> atr);
    vector<float> treinar(Corpus &corpus, int atributo );
    float calculaCustoLog2(float **X, int *y, float *theta, int m, int n);
  void gradientDescentLog22(float **X, int *y, float *theta, int m, int n, float alpha, int num_iters);

};

#endif //

