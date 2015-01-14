#ifndef CLASSIFICADORREGLOG_H
#define CLASSIFICADORREGLOG_H

#include <map>
#include <string>
#include <vector>
#include "../outros/auxfunc.h"
#include "../treinador/treinador.h"
#include "../corpus/corpus.h"
#include "../treinador/RegressaoLogistica.h"


class ClassificadorRegLog:public Classificador
{
private:
    vector<float> theta;
    vector<string> classes;
    vector<string> atributos;

public:
    ClassificadorRegLog(vector<string> classes);
    ClassificadorRegLog(){
    };

    virtual ~ClassificadorRegLog();
    virtual bool executarClassificacao(Corpus&, int);
    //void ajustarTheta2(vector<float> theta, int n);
    void ajustarTheta2(float *theta, int n);
    void ajustarClasses(vector<string> classes);
    void ajustarAtributos(vector<string> atributos, int n);

    bool gravarConhecimento(string)
    {
        throw "Não Implementado";
    }
    bool carregarConhecimento(string)
    {
        throw "Não Implementado";
    }

protected:

};

#endif // CLASSIFICADORREGLOG_H
