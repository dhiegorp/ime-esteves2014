#include "avaliador_acuracia.h"

AvaliadorAcuracia::AvaliadorAcuracia()
{
    //ctor
}

AvaliadorAcuracia::~AvaliadorAcuracia()
{
    //dtor
}

vector<float> AvaliadorAcuracia::calcularDesempenho( Corpus &corpus, int atributo_padrao, int atributo_teste )
{
    /**
    *
    *   Calcula a porcentagem de acerto para um determinado atributo
    *   Diego: retornando a quantidade de acertos por classe (0/1) (util quando analisar false pos / true pos)
    *          [0] = acurácia
    *          [1] = tot ocorrencias classe 0
    *          [2] = tot ocorrencias classe 1
    *          [3] = tot acertos classe 0 = vn
    *          [4] = tot acertos classe 1 = vp
    *          [5] = prev -1, real +1 = fn
    *          [6] = prev +1, real -1 = fp
    */
    vector<float> vectorAcuracia;
    int row = corpus.pegarQtdSentencas(), column, acertos = 0, totalTokens = 0, ocorc0 = 0, ocorc1 = 0, acertosc0 = 0, acertosc1=0;
    int fn=0, fp=0;
    int vattpadrao, vattteste;

    for ( register int i = 0; i < row; ++i )
    {
        column = corpus.pegarQtdTokens( i );

        for ( register int j = 0; j < column; ++j )
        {

            vattpadrao = corpus.pegarValor(i,j,atributo_padrao);
            vattteste = corpus.pegarValor(i,j,atributo_teste);

            string svattpadrao = corpus.pegarSimbolo(vattpadrao);
            string svattteste = corpus.pegarSimbolo(vattteste);

            if (svattpadrao == "+1") {
                ++ocorc1;
            }
            else{
                ++ocorc0;
            }

            if ( vattpadrao == vattteste ){
                ++acertos;
                if (svattpadrao == "+1") {
                    ++acertosc1;
                }
                else{
                    ++acertosc0;
                }
            }else{
                 if ((svattpadrao == "+1") && (svattteste == "-1")){
                    ++fn;
                 }else{++fp;}
            }

            ++totalTokens;
        }
    }


    vectorAcuracia.push_back( (float)acertos / totalTokens );
    vectorAcuracia.push_back( (float)ocorc0 );   //ocorrencias da classe 0
    vectorAcuracia.push_back( (float)ocorc1 );   //ocorrencias da classe 1
    vectorAcuracia.push_back( (float)acertosc0 ); // acertos na classe 0 = verdadeiro negativo (real -1, prev -1)
    vectorAcuracia.push_back( (float)acertosc1 ); // acertos na classe 1 = // verdadeiro positivo (real +1, prev +1)

    vectorAcuracia.push_back( (float)fn ); // falso negativo (real+1, prev -1)
    vectorAcuracia.push_back( (float)fp ); // falso positivo (real -1, prev +1)




    return vectorAcuracia;
}

vector<float> AvaliadorAcuracia::calcularDesempenho2( Corpus &corpus, int atributo_padrao, int atributo_teste, int atributo_considerar )
{
    /**
    *
    *   Calcula a porcentagem de acerto para um determinado atributo
    *
    */
    vector<float> vectorAcuracia;
    int row = corpus.pegarQtdSentencas(), column, acertos = 0, totalTokens = 0;
    bool considerar = false;

    for ( register int i = 0; i < row; ++i )
    {
        column = corpus.pegarQtdTokens( i );

        for ( register int j = 0; j < column; ++j )
        {
            if ( corpus.pegarValor(i,j,atributo_padrao) == corpus.pegarValor(i,j,atributo_teste) )
                ++acertos;

            if (corpus.pegarValor(i,j,atributo_considerar) == corpus.pegarIndice("1")) {
                considerar=true;
            }
            else
            {
                considerar=false;
            }

            ++totalTokens;

        }
    }

    if (!considerar){
        vectorAcuracia.push_back( -1 );
    }else{
        vectorAcuracia.push_back( (float)acertos / totalTokens );
    }


    return vectorAcuracia;
}
