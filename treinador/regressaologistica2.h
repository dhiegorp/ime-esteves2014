#ifndef REGRESSAOLOGISTICA_H
#define REGRESSAOLOGISTICA_H

#include <map>
#include <vector>
#include "treinador.h"

using namespace std;

class ClassificadorRegressaoLogistica2:public Classificador{
    private:
        vector<float> theta;
        vector<string> classes;
    public:
        ClassificadorRegressaoLogistica2(){
        }
        void ajustarTheta2(float *theta, int n){
            this->theta.resize(n);
            for (int i = 0; i < n; i++)
                this->theta[i] = theta[i];
        }
        ~ClassificadorRegressaoLogistica2(){
            theta.resize(0);
        }
        virtual bool executarClassificacao(Corpus&, int);
        void ajustarClasses2(vector<string> classes){
            this->classes = classes;
        }
        bool gravarConhecimento(string){throw "Não Implementado";}
        bool carregarConhecimento(string){throw "Não Implementado";}
};


class RegressaoLogistica2 : public Treinador
{
    vector<string> classes;
    public:
        RegressaoLogistica2(vector<string> classes);
        virtual ~RegressaoLogistica2();
        virtual Classificador* executarTreinamento( Corpus &corpus, int atributo );
};

#endif // MAISPROVAVEL_H
