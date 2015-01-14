#include <map>
#include <vector>
#include "../treinador/treinador.h"
#include "../classificador/classificadorregressaologistica.h"

using namespace std;

class RegressaoLogistica:public Treinador{
    vector<string> classes, atributos;
    public:
        RegressaoLogistica(vector<string>, vector<string>);
        Classificador* executarTreinamento( Corpus &corpus, int atributo );
};

