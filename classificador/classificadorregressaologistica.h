#include "../classificador/classificador.h"

//prototipo do classificador
class ClassificadorRegressaoLogistica:public Classificador{
    private:
        vector <string> classes, atributos;
        float* theta;
    public:
        ClassificadorRegressaoLogistica(vector<string> clas, vector<string> atr, float* theta);
        bool executarClassificacao( Corpus &corpusProva, int atributo );
        bool gravarConhecimento( string arquivo ){return true;}
        bool carregarConhecimento( string arquivo ){return true;}
       string descricaoConhecimento();
};

