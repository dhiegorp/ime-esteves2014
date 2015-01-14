#include "reglog.h"
#include "../classificador/ClassificadorRegLog.h"
#include "../classificador/classificador.h"
#include "../outros/auxfunc.h"
#include "../corpus/corpusmatriz.h"
#include "../corpus/corpus.h"
#include <string>
#include <sstream>
#include <iostream>

#include <math.h>
#include <stdlib.h>

RegLog::RegLog(vector<string> classes, int iter, float alpha, float lamb, float Jmin, float const_e)
{
    this->classes = classes;
    this->iter = iter;
    this->alpha = alpha;
    this->lamb = lamb;
    this->Jmin = Jmin;
    this->const_e = const_e;
}

RegLog::~RegLog()
{
    //dtor
}

void RegLog::ajustarParametros(vector<string> atr)
{

    this->atributos = atr; //vetor de atributos

}

//Classificador*  RegLog::executarTreinamento( Corpus &corpus, int atributo )
//{
//    // = new ClassificadorRegressaoLogistica(vector<string> classes);
//    ClassificadorRegLog *objClassificador;
//    vector<float> thetas_treino, thetas_min, thetas_atuais; //vetores auxiliares com valores de theta
//    vector<float> vetY, Xj;                                 //vetores auxiliares de Y e das colunas com atributos
//    vector<float> residuo, residuo_aux, residuo_pow;
//    float sum_residuo, sum_residuo_aux, sum_residuo_pow;
//    float qtdAtributos;                                     //total de atributos no treinamento atual (janela)
//    float nExemplos;                                        //total de registros de exemplo
//    float v,valY,hx1,thetaZero, pol,e2,theta2,somaHx1,somaHx0,somaHx2,Jatual,valX;
//    unsigned int iatr;
//    int nConjExemplos,c,e,a;
//    int nTotalExemplos = 0;
//
//    qtdAtributos = atributos.size();
//
//
//    thetas_treino.resize(qtdAtributos+1);
//    thetas_min.resize(qtdAtributos+1);
//    thetas_atuais.resize(qtdAtributos+1);
//
//    Xj.resize(qtdAtributos);
//    residuo.resize(qtdAtributos);
//
//
//    //gerando thetas iniciais aleatórios
//    for (int i=0; i<=qtdAtributos; i++)
//    {
//        thetas_treino[i]= (float)rand()/(float)RAND_MAX;
//    }
//
//    nConjExemplos = corpus.pegarQtdConjExemplos();
//
//    vetY.resize(nConjExemplos);
//
//    //popular vetor de saida (y)
//     for (c=0; c < nConjExemplos; c++){
//        nExemplos = corpus.pegarQtdExemplos(c);
//        for (e = 0; e < nExemplos; e++){
//             v = corpus.pegarValor(c, e, atributo);
//             (std::istringstream)(corpus.pegarSimbolo(v)) >> valY >> std::dec;
//             vetY[c] = valY;
//        }
//     }
//
//
//    // calculando o gradiente descedente. realiza g iterações em busca no menor erro global
//    for (int g=1; g< iter; g++)
//    {
//        for (c=0; c < nConjExemplos; c++){
//        nExemplos = corpus.pegarQtdExemplos(c);
//            for (e = 0; e < nExemplos; e++){
//                //(T0 + (T1 * X1_i) + (T2 * X2_i) + (TN * XN_i)) - Y_i
//                for (int a=0; a<qtdAtributos-1; a++) {
//
//                    iatr = corpus.pegarPosAtributo(atributos[a]);
//                    v = corpus.pegarValor(c, e, iatr);
//                    (std::istringstream)(corpus.pegarSimbolo(v)) >> valX >> std::dec;
//
//                    if (a==0) {
//                        residuo[c] += thetas_treino[a];
//                    }
//                    else
//                    {
//                        residuo[c] += thetas_treino[a] * valX[a];
//                    }
//
//                    residuo_aux[c] = residuo[c] * valX;
//                    residuo_pow[c] = pow(residuo[c],2);
//
//                    sum_residuo += residuo[c];
//                    sum_residuo_aux += residuo_aux[c];
//                    sum_residuo_pow += residuo_pow[c];
//
//                }
//            }
//        }
//
//        Jatual = ((1/2) * lamb)  * sum_residuo_pow;
//
//
//
//
//
//
//
//                //    Xj[a]= valX;
//                //    pol = pol + (  (Xj[a]) * (thetas_treino[a+1]));
//               // }
//
//                //soma dos (h(x)-y)*Xj e (h(x)-y)
//                pol += thetaZero;
//                pol=pol*(-1);
//
//                e2= pow(const_e,pol);
//
//                somaHx0 += ((1/(1+e2))- vetY[c]);
//                somaHx2 += (pow(((1/(1+e2))- vetY[c]),2));
//                somaHx1 += (((1/(1+e2))- vetY[c])*Xj[0]);
//
//                pol=0;
//                e2=0;
//                hx1=0;
//            }
//        // ***************************************************************
//
//        //calculando o polinomio que será expoente de e, com os thetas iniciais
//        //inserindo theta0 e calculando theta2(quadradado)
//        thetaZero= thetas_treino[0];
//
//        for(int b=1; b<qtdAtributos; b++)
//        {
//            theta2 += (pow(thetas_treino[b],2));
//        }
//
//
//        //calculando  custo minimo.
//        Jatual =((((1/(2*nConjExemplos))*(somaHx2)))+(lamb*(nConjExemplos*theta2)));
//
//        //gravando o menor erro e melhores thetas;
//        if (Jatual < Jmin)
//        {
//            Jmin =Jatual;
//            thetas_min = thetas_treino;
//            //cout<< thetas_treino[i] << "\n ";
//        }
//
//        //calculando proximos theta 0..n
//        thetas_atuais[0] = (thetas_treino[0] - ((alpha*(1/nConjExemplos))*(somaHx0)));
//
//        for (int a=1; a< qtdAtributos; a++)
//        {
//            thetas_atuais[a] = ((thetas_treino[a] *(1-(alpha*(lamb/nConjExemplos))))-(alpha*((1/nConjExemplos)*(somaHx1))));
//
//        }
//
//        somaHx0=0;
//        somaHx1=0;
//        somaHx2=0;
//        Jatual=0;
//        theta2=0;
//
//        thetas_treino = thetas_atuais;
//
//    objClassificador = new ClassificadorRegLog(classes);
//    objClassificador->ajustarTheta(thetas_min, qtdAtributos+1); //atualiza com os valores de thetas encontrados no menor custo
//    objClassificador->ajustarAtributos(atributos, qtdAtributos); //atualiza com os atributos da janela atual
//    //objClassificador->ajustarClasses(classes);
//    return objClassificador;
//}

Classificador* RegLog::executarTreinamento( Corpus &corpus, int atributo )
{
    //ClassificadorRegLog *objClassificador = new ClassificadorRegLog;
    ClassificadorRegLog *objClassif = new ClassificadorRegLog;

    float **X, *theta, val;
    int *y, i, j, m, v, n;
    map<string, int> classeMap;

    classeMap[classes[0]] = 0;
    classeMap[classes[1]] = 1;

    n = atributo + 1;
    m = corpus.pegarQtdConjExemplos();
    X = new float*[m];
    y = new int[m];

    theta = new float[n];
    for (i=0; i<m; i++){
        X[i] = new float[n];
        X[i][0] = 1.0;
        for (j=0; j<atributo;j++){
            v = corpus.pegarValor(i, 0, j);

            (std::istringstream)(corpus.pegarSimbolo(v)) >> val >> std::dec;//converte para float
            X[i][j + 1] = val;
        }
        v = corpus.pegarValor(i, 0, atributo);

        y[i] = classeMap[corpus.pegarSimbolo(v)];
    }


//    gradientDescentLog22(X, y, theta, m, n, 1, 1);

//    this->teste();

    objClassif->ajustarClasses(classes);
    objClassif->ajustarTheta2(theta, n);
    //cout << "\n" << m << " " << calculaCustoLog(X, y, theta, m, n) << "\n" << theta[0] << ", " << theta[1] << ", " << theta[2];
    delete[] theta;
    delete[] y;
    for (i=0; i<m; i++)
        delete[] X[i];
    delete[] X;

    return objClassif;
}

float calculaCustoLog2(float **X, int *y, float *theta, int m, int n){
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

void gradientDescentLog22(float **X, int *y, float *theta, int m, int n, float alpha = 0.01, int num_iters = 1500){
    int iter, i, j;
    float *HO, *sum, A;

    HO = new float[m];
    sum = new float[n];

    for (j=0; j<n;j++)
        theta[j] = 0.0;

    for (iter = 0; iter < num_iters; iter++){
        matrixVetMulti(X, theta, HO, m, n);
        inplace_sigmoid(HO, m);

        for (j=0; j<n;j++)
            sum[j] = 0.0;

        for (i = 0; i < m; i++){
            A = HO[i] - y[i];
            for (j=0; j<n;j++)
                sum[j] += A*X[i][j];
        }

        for (j=0; j<n;j++)
            theta[j] -= alpha*sum[j]/m;
    }

    delete[] HO;
    delete[] sum;
}

bool ClassificadorRegLog::executarClassificacao(Corpus &corpus, int atributo){
    int m, i, j, v, n;
    float total, val;
    string c;

    m = corpus.pegarQtdConjExemplos();//numero de exemplos
    n = theta.size();//numero de atributos + 1

    for (i=0; i<m; i++){
        total = 1.0*theta[0];
        for (j=0; j < n-1; j++){
            v = corpus.pegarValor(i, 0, j);

            (std::istringstream)(corpus.pegarSimbolo(v)) >> val >> std::dec;//converte para float
            total += theta[j + 1]*val;
        }

        c = classes[(sigmoid(total) >= 0.5)?1:0];
        corpus.ajustarValor(i, 0, atributo, corpus.pegarIndice(c));
    }
    return true;
}

//float RegLog::calculaCustoLog(float **X, int *y, float *theta, int m, int n)
//{
//    float ret=0;
//    return ret;
//}
//
//void RegLog::gradientDescentLog(float **X, int *y, float *theta, int m, int n, float alpha = 0.01, int num_iters = 1500)
//{
//
//}

//void teste(){
//}

