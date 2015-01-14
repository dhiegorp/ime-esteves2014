#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <math.h>

#include "../processador/processadorsh.h"
#include "../corpus/corpusmatriz.h"
#include "../avaliador/avaliador_acuracia.h"
#include "../treinador/naivebayes.h"
#include "../treinador/libsvm.h"
#include "../treinador/regressaologistica.h"
#include "../treinador/reglog.h"
#include "../validador/validadorkdobras.h"
#include "../validador/validadorkdobras_deslizante.h"
#include "../classificador/classificadorbls.h"
using namespace std;

/**************************** DECLARAÇÃO DE VARIÁVEIS *****************************/

vector<string> atributosTreino;             //representa os atributos de treinamento (valor, fechamento, abertura, d-1, d-2...)
int iResposta;
int iSaidaNB;
int iSaidaBLS;
int iSaidaSVM;
int iSaidaRegLog;
float acuracia = 0.0;
bool svm, nb, bls, reglog;

bool svm_validacao_cruzada1;
bool svm_validacao_cruzada2;
bool svm_validacao_cruzada3;
bool reglog_validacao_cruzada1;
bool reglog_validacao_cruzada2;
bool reglog_validacao_cruzada3;
bool nb_validacao_cruzada1;
bool nb_validacao_cruzada2;
bool nb_validacao_cruzada3;

AvaliadorAcuracia objAvalAcur;
stringstream out;
struct svm_parameter param;                 //auxiliar para integracao com SVM
int iter;                                   // auxiliar regressao logistica
unsigned int i, j;                          //variaveis auxiliares
string auxexecbls;                          //manter o valor do BLS
vector<int> valores_janelas(8);             //8 - vetor das janelas a serem testadas
vector<string> valores_codigoativo(1);       //35-45-50 - vetor dos ativos a serem testados
vector<string> classes;
int janela_max;
float c,soma_acuracia = 0;
int nextid;

/* true positive / false positive analysis */
//trainning set
int _tot_oc_C0 = 0, _tot_oc_C1 = 0;

//test set
int _tot_oc_test_C0 = 0,   _tot_oc_test_C1 = 0;   //sliding mode
int _tot_oc_test1_C0 = 0,  _tot_oc_test1_C1 = 0;  //cross-validation k-fold
int _tot_oc_test3_C0 = 0,  _tot_oc_test3_C1 = 0;  //80%20%

//validating model (hits on test)
int _tot_hits_C0_svm = 0,  _tot_hits_C1_svm = 0;  //sliding mode (exp2)
int _tot_hits_C0_rl = 0,   _tot_hits_C1_rl = 0;   //sliding mode (exp2)
int _tot_hits_C0_nb = 0,   _tot_hits_C1_nb = 0;   //sliding mode (exp2)

int _tot_hits1_C0_svm = 0, _tot_hits1_C1_svm = 0; //cross-validation k-fold (exp1)
int _tot_hits1_C0_rl = 0,  _tot_hits1_C1_rl = 0;  //cross-validation k-fold (exp1)
int _tot_hits1_C0_nb = 0,  _tot_hits1_C1_nb = 0;  //cross-validation k-fold (exp1)

int _tot_hits3_C0_svm = 0, _tot_hits3_C1_svm = 0; //80%20% (exp3)
int _tot_hits3_C0_rl = 0,  _tot_hits3_C1_rl = 0;  //80%20% (exp3)
int _tot_hits3_C0_nb = 0,  _tot_hits3_C1_nb = 0;  //80%20% (exp3)

//int _exp2_vp_svm = 0, _exp2_fn_svm = 0, _exp2_fp_svm =0, _exp2_vn_svm = 0;//sliding mode (exp2)
//int _exp2_vp_nb = 0, _exp2_fn_nb = 0, _exp2_fp_nb =0, _exp2_vn_nb = 0;//sliding mode (exp2)
//int _exp2_vp_rl = 0, _exp2_fn_rl = 0, _exp2_fp_rl =0, _exp2_vn_rl = 0;//sliding mode (exp2)
//
//int _exp3_vp_svm = 0, _exp3_fn_svm = 0, _exp3_fp_svm =0, _exp3_vn_svm = 0;//80%20% (exp3)
//int _exp3_vp_nb = 0, _exp3_fn_nb = 0, _exp3_fp_nb =0, _exp3_vn_nb = 0;//80%20% (exp3)
//int _exp3_vp_rl = 0, _exp3_fn_rl = 0, _exp3_fp_rl =0, _exp3_vn_rl = 0;//80%20% (exp3)

int  _exp2_fn_svm = 0, _exp2_fp_svm =0 ;//sliding mode (exp2)
int  _exp2_fn_nb = 0, _exp2_fp_nb =0 ;//sliding mode (exp2)
int  _exp2_fn_rl = 0, _exp2_fp_rl =0 ;//sliding mode (exp2)

int  _exp3_fn_svm = 0, _exp3_fp_svm = 0;//80%20% (exp3)
int  _exp3_fn_nb = 0, _exp3_fp_nb = 0;//80%20% (exp3)
int  _exp3_fn_rl = 0, _exp3_fp_rl = 0;//80%20% (exp3)

//inserts banco de dados
string _cod_ativo,_param1_rl,_param2_rl,_param3_rl, _dt_inicio_periodo, _dt_fim_periodo, _out_svm_exp1_description, _out_svm_exp2_description;
string _out_lr_exp1_description, _out_lr_exp2_description, _out_nb_exp1_description, _out_nb_exp2_description;
int _janela,_dobras,_tam_janela_desl,_param1_svm,_param2_svm, _total_dias_serie, _serie_final_lenght, _records_used_adjustment_factor, _scaled_data;
float _perc_treino_exp3,_out_bls,_out_svm,_out_svm_exp1,_out_svm_exp2,_out_svm_exp3,_out_nb,_out_nb_exp1,_out_nb_exp2,_out_nb_exp3;
float _out_rl,_out_rl_exp1,_out_rl_exp2,_out_rl_exp3,_perc_fat_ajuste,_param3_svm,_param4_svm,_param5_svm,_param6_svm;
float _param7_svm,_param8_svm,_param9_svm,_param10_svm,_param11_svm,_param12_svm,_param13_svm;
bool _maximo,_minimo,_fechamento,_abertura,_volume,_fat_ajuste, _quantidade;


void show_error(unsigned int handletype, const SQLHANDLE& handle){
    SQLCHAR sqlstate[1024];

    SQLCHAR message[1024];
    if(SQL_SUCCESS == SQLGetDiagRec(handletype, handle, 1, sqlstate, NULL, message, 1024, NULL))
        cout<<"Message: "<<message<<"\nSQLSTATE: "<<sqlstate<<endl;

    cout << '\a';
    cout << '\a';
    cout << '\a';
    cout << '\a';
    cout << '\a';
    cout << '\a';

    getchar();

}


int getNextId(){

    SQLHANDLE sqlenvhandle, sqlconnectionhandle, sqlstatementhandle;
//    SQLRETURN retcode;
//    SQLINTEGER id_tmp;

     if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLSetEnvAttr(sqlenvhandle,SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &sqlconnectionhandle))
        goto FINISHED;

    SQLCHAR retconstring[1024];
    switch(SQLDriverConnect (sqlconnectionhandle,
                NULL,
                (SQLCHAR*)"DRIVER={SQL Server};SERVER=ULTRA\\MSSQLSERVERULTRA;DATABASE=bovespa;UID=sa;PWD=nvclts@7;",
                SQL_NTS,
                retconstring,
                1024,
                NULL,
                SQL_DRIVER_NOPROMPT)){
        case SQL_SUCCESS_WITH_INFO:
            //show_error(SQL_HANDLE_DBC, sqlconnectionhandle);
            break;
        case SQL_INVALID_HANDLE:
        case SQL_ERROR:
            show_error(SQL_HANDLE_DBC, sqlconnectionhandle);
            goto FINISHED;
        default:
            break;
    }

    if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLExecDirect(sqlstatementhandle, (SQLCHAR*)"SELECT CASE WHEN max(execution_group_id) IS NULL THEN '0' ELSE max(execution_group_id) END AS MAX_ID FROM RESULTS", SQL_NTS)){
        show_error(SQL_HANDLE_STMT, sqlstatementhandle);
        goto FINISHED;
    }

    else{
        //char name[64];
        //char address[64];
        int id;
        while(SQLFetch(sqlstatementhandle)==SQL_SUCCESS){
            SQLGetData(sqlstatementhandle, 1, SQL_C_ULONG, &id, 0, NULL);
            //SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, name, 64, NULL);
            //SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, address, 64, NULL);
           return id+1;

        }
    }

FINISHED:
    SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );
    SQLDisconnect(sqlconnectionhandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);
    return -1;
}

void insertOnDB(int janela, string cod_ativo,
    float out_bls,
    float out_svm, float out_svm_exp1, float out_svm_exp2, float out_svm_exp3, float out_nb, float out_nb_exp1,
    float out_nb_exp2, float out_nb_exp3, float out_rl, float out_rl_exp1, float out_rl_exp2, float out_rl_exp3,
    int dobras, int tam_janela_desl, float perc_treino_exp3,bool maximo, bool minimo, bool fechamento, bool abertura, bool volume,
    bool fat_ajuste, float perc_fat_ajuste, string  param1_rl, string  param2_rl, string param3_rl, int param1_svm, int param2_svm,
    float param3_svm, float param4_svm, float param5_svm, float param6_svm, float param7_svm, float param8_svm, float param9_svm,
    float param10_svm, float param11_svm, float param12_svm, float param13_svm, int total_dias_serie, string dt_inicio_periodo, string dt_fim_periodo,
    int id_execution, int records_used_adjustment_factor, int serie_final_lenght, int scaled_data, int tot_oc_C0, int tot_oc_C1, int tot_hits_C0_svm,
    int tot_hits_C1_svm,int tot_hits_C0_rl,int tot_hits_C1_rl,int tot_hits_C0_nb,int tot_hits_C1_nb, int tot_oc_C0_test, int tot_oc_C1_test,
    int tot_hits1_C0_svm,int tot_hits1_C1_svm,int tot_hits1_C0_rl,int tot_hits1_C1_rl,int tot_hits1_C0_nb,int tot_hits1_C1_nb, int tot_oc_C0_test1, int tot_oc_C1_test1,
    int tot_hits3_C0_svm,int tot_hits3_C1_svm,int tot_hits3_C0_rl,int tot_hits3_C1_rl,int tot_hits3_C0_nb,int tot_hits3_C1_nb, int tot_oc_C0_test3, int tot_oc_C1_test3, bool quantidade,
    int exp2_fn_svm,int exp2_fp_svm, int exp2_fn_nb, int exp2_fp_nb, int exp2_fn_rl, int exp2_fp_rl,
    int exp3_fn_svm,int exp3_fp_svm, int exp3_fn_nb, int exp3_fp_nb, int exp3_fn_rl, int exp3_fp_rl)
    {
    SQLHANDLE sqlenvhandle, sqlconnectionhandle, sqlstatementhandle;
    //SQLRETURN retcode;

    std::stringstream out;
    out << "insert into results(window, asset_code, out_bls," << "out_svm, out_svm_exp1, out_svm_exp2, out_svm_exp3, ";
    out << "out_nb, out_nb_exp1, out_nb_exp2, out_nb_exp3, " << "out_lr, out_lr_exp1, out_lr_exp2, out_lr_exp3, ";
    out << "param_sys_fold_exp1, param_sys_size_sliding_window_exp2," << "param_sys_perc_train_exp3, param_sys_att_high,";
    out << "param_sys_att_low, param_sys_att_close," << "param_sys_att_open, param_sys_att_tot_volume,";
    out << "param_sys_adjustment_factor, param_sys_adjustment_factor_perc," << "param1_lr, param2_lr, param3_lr,";
    out << "param1_svm_type, param2_svm_kernel, param3_svm_degree," << "param4_svm_gamma, param5_svm_coef0, param6_svm_C,";
    out << "param7_svm_nu, param8_svm_p, param9_svm_cache_size," << "param10_svm_eps, param11_svm_shrinking, param12_svm_probability,";
    out << "param13_svm_nr_weight, serie_total_lenght, start_period, end_period, execution_group_id, records_used_adjustment_factor,";
    out << "serie_final_lenght, scaled_data,  tot_occurrences_train_C0, tot_occurrences_train_C1, tot_hits_C0_svm, tot_hits_C1_svm,";
    out << "tot_hits_C0_rl, tot_hits_C1_rl, tot_hits_C0_nb, tot_hits_C1_nb,tot_occurrences_test_C0, tot_occurrences_test_C1,";
    out << "tot_hits1_C0_svm,tot_hits1_C1_svm,tot_hits1_C0_rl,tot_hits1_C1_rl,tot_hits1_C0_nb,tot_hits1_C1_nb,tot_occurrences_test1_C0, tot_occurrences_test1_C1,";
    out << "tot_hits3_C0_svm,tot_hits3_C1_svm,tot_hits3_C0_rl,tot_hits3_C1_rl,tot_hits3_C0_nb,tot_hits3_C1_nb,tot_occurrences_test3_C0, tot_occurrences_test3_C1, param_sys_att_tot_quantidade,";
    out << "exp2_fn_svm, exp2_fp_svm, exp2_fn_nb, exp2_fp_nb, exp2_fn_rl, exp2_fp_rl, ";
    out << "exp3_fn_svm, exp3_fp_svm, exp3_fn_nb, exp3_fp_nb, exp3_fn_rl, exp3_fp_rl";

    out << ")values (" << janela << ",'" << cod_ativo << "'," << out_bls << "," << out_svm << "," << out_svm_exp1 << ",";
    out << out_svm_exp2 << "," << out_svm_exp3 << "," << out_nb << "," << out_nb_exp1 << "," << out_nb_exp2 << "," << out_nb_exp3 << ",";
    out << out_rl << "," << out_rl_exp1 << "," << out_rl_exp2 << "," << out_rl_exp3 << "," << dobras << "," << tam_janela_desl << ",";
    out << perc_treino_exp3 << "," << maximo << "," << minimo << "," << fechamento << "," << abertura << "," << volume << ",";
    out << fat_ajuste << "," << perc_fat_ajuste << ",'" << param1_rl << "','" << param2_rl << "','" << param3_rl << "',";
    out << param1_svm << "," << param2_svm << "," << param3_svm << "," << param4_svm << "," << param5_svm << "," << param6_svm << ",";
    out << param7_svm << "," << param8_svm << "," << param9_svm << "," << param10_svm << "," << param11_svm << "," << param12_svm << "," << param13_svm << "," ;
    out << total_dias_serie << ",'" << dt_inicio_periodo << "','" << dt_fim_periodo << "'," << id_execution << "," << records_used_adjustment_factor << "," ;
    out << serie_final_lenght << "," << scaled_data << "," << tot_oc_C0 << "," << tot_oc_C1 << "," << tot_hits_C0_svm << "," << tot_hits_C1_svm << "," ;
    out << tot_hits_C0_rl << "," << tot_hits_C1_rl << "," << tot_hits_C0_nb << "," << tot_hits_C1_nb << "," << tot_oc_C0_test << "," << tot_oc_C1_test << ",";
    out << tot_hits1_C0_svm << "," << tot_hits1_C1_svm << "," << tot_hits1_C0_rl << "," << tot_hits1_C1_rl << "," << tot_hits1_C0_nb << "," << tot_hits1_C1_nb << "," << tot_oc_C0_test1 << "," << tot_oc_C1_test1 << ",";
    out << tot_hits3_C0_svm << "," << tot_hits3_C1_svm << "," << tot_hits3_C0_rl << "," << tot_hits3_C1_rl << "," << tot_hits3_C0_nb << "," << tot_hits3_C1_nb << "," << tot_oc_C0_test3 << "," << tot_oc_C1_test3 << "," << quantidade << ",";
    out << exp2_fn_svm<< "," << exp2_fp_svm<< "," <<  exp2_fn_nb<< "," << exp2_fp_nb<< "," << exp2_fn_rl<< "," << exp2_fp_rl<< "," ;
    out << exp3_fn_svm<< "," << exp3_fp_svm<< "," <<  exp3_fn_nb<< "," << exp3_fp_nb<< "," << exp3_fn_rl<< "," << exp3_fp_rl ;
    out << ")";

    const std::string tmp = out.str();
    const char* cstr = tmp.c_str();

    //cout << tmp.c_str();


    if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLSetEnvAttr(sqlenvhandle,SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &sqlconnectionhandle))
        goto FINISHED;

    SQLCHAR retconstring[1024];
    switch(SQLDriverConnect (sqlconnectionhandle,
                NULL,
                (SQLCHAR*)"DRIVER={SQL Server};SERVER=ULTRA\\MSSQLSERVERULTRA;DATABASE=bovespa;UID=sa;PWD=nvclts@7;",
                SQL_NTS,
                retconstring,
                1024,
                NULL,
                SQL_DRIVER_NOPROMPT)){
        case SQL_SUCCESS_WITH_INFO:
            //show_error(SQL_HANDLE_DBC, sqlconnectionhandle);
            break;
        case SQL_INVALID_HANDLE:
        case SQL_ERROR:
            show_error(SQL_HANDLE_DBC, sqlconnectionhandle);
            goto FINISHED;
        default:
            break;
    }

    if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
        goto FINISHED;

    if(SQL_SUCCESS!=SQLExecDirect(sqlstatementhandle, (SQLCHAR*)cstr, SQL_NTS)){
        show_error(SQL_HANDLE_STMT, sqlstatementhandle);
        goto FINISHED;
    }

    else{
        char name[64];
        char address[64];
        int id;
        while(SQLFetch(sqlstatementhandle)==SQL_SUCCESS){
            SQLGetData(sqlstatementhandle, 1, SQL_C_ULONG, &id, 0, NULL);
            SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, name, 64, NULL);
            SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, address, 64, NULL);
            cout<<id<<" "<<name<<" "<<address<<endl;
        }
    }

FINISHED:
    SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );
    SQLDisconnect(sqlconnectionhandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);

}



int main()
{
 /*
    Estrutura do Corpus
    1:   valor
    2:   d-1
    3:   d-2
    n:   d-n
    n+1: y (+1,-1)
    n+2: saida_nb (+1,-1,0)
    n+3: saida_bls (+1,-1,0)
    n+4: saida_svm (+1,-1,0)
    n+5: saida_reglog (+1,-1,0)
*/

    cout << '\a';

    /**************************** PARAMETROS DE ENTRADA DO PROGRAMA *****************************/
    valores_janelas[0] = 3; valores_janelas[1] = 5;  valores_janelas[2] = 10; valores_janelas[3] = 15;
    valores_janelas[4] = 20; valores_janelas[5] = 40; valores_janelas[6] = 60; valores_janelas[7] = 80;


        valores_codigoativo[0] = "ELET6"; //ALLL11
        //valores_codigoativo[1] = "CSNA3";
        //valores_codigoativo[2] = "PETR4";
        //valores_codigoativo[3] = "ELET6";

//        valores_codigoativo[0] = "ALLL11";
//        valores_codigoativo[1] = "PETR4";
//        valores_codigoativo[2] = "VALE5";
//        valores_codigoativo[3] = "ARCZ6";
//        valores_codigoativo[4] = "BBAS3";
//        valores_codigoativo[5] = "BBDC4";
//        valores_codigoativo[6] = "BNCA3";
//        valores_codigoativo[7] = "BRAP4";
//        valores_codigoativo[8] = "BRKM5";
//        valores_codigoativo[9] = "CCRO3";
//        valores_codigoativo[10] = "CLSC6";
//        valores_codigoativo[11] = "CPFE3";
//        valores_codigoativo[12] = "CRUZ3";
//        valores_codigoativo[13] = "CSAN3";
//        valores_codigoativo[14] = "CSNA3";
//        valores_codigoativo[15] = "CYRE3";
//        valores_codigoativo[16] = "DURA4";
//        valores_codigoativo[17] = "EMBR3";
//        valores_codigoativo[18] = "GGBR4";
//        valores_codigoativo[19] = "GOLL4";
//        valores_codigoativo[20] = "ITAU4";
//        valores_codigoativo[21] = "ITSA4";
//        valores_codigoativo[22] = "KLBN4";
//        valores_codigoativo[23] = "LREN3";
//        valores_codigoativo[24] = "NATU3";
//        valores_codigoativo[25] = "NETC4";
//        valores_codigoativo[26] = "RSID3";
//        valores_codigoativo[27] = "SDIA4";
//        valores_codigoativo[28] = "TAMM4";
//        valores_codigoativo[29] = "TLPP4";
//        valores_codigoativo[30] = "TNLP4";
//        valores_codigoativo[31] = "UBBR11";
//        valores_codigoativo[32] = "UGPA4";
//        valores_codigoativo[33] = "USIM5";
//        valores_codigoativo[34] = "VCPA4";

    /*valores_codigoativo[0] = "ARCZ6"; valores_codigoativo[1] = "PETR4";   valores_codigoativo[2] = "ELET6";
    valores_codigoativo[3] = "CSNA3";  valores_codigoativo[4] = "VALE5";   valores_codigoativo[5] = "ALLL11";
    valores_codigoativo[6] = "AMBV4";  valores_codigoativo[7] = "BBAS3";   valores_codigoativo[8] = "BBDC4";
    valores_codigoativo[9] = "BNCA3";  valores_codigoativo[10] = "BRAP4";  valores_codigoativo[11] = "BRKM5";
    valores_codigoativo[12] = "BRTO4"; valores_codigoativo[13] = "CCRO3";  valores_codigoativo[14] = "CGAS5";
    valores_codigoativo[15] = "CLSC6"; valores_codigoativo[16] = "CMIG4";  valores_codigoativo[17] = "CPFE3";
    valores_codigoativo[18] = "CPLE6"; valores_codigoativo[19] = "CRUZ3";  valores_codigoativo[20] = "CSAN3";
    valores_codigoativo[21] = "VCPA4"; valores_codigoativo[22] = "CYRE3";  valores_codigoativo[23] = "DURA4";
    valores_codigoativo[24] = "USIM5"; valores_codigoativo[25] = "EMBR3";  valores_codigoativo[26] = "GGBR4";
    valores_codigoativo[27] = "GOLL4"; valores_codigoativo[28] = "ITAU4";  valores_codigoativo[29] = "ITSA4";
    valores_codigoativo[30] = "KLBN4"; valores_codigoativo[31] = "LAME4";  valores_codigoativo[32] = "LREN3";
    valores_codigoativo[33] = "NATU3"; valores_codigoativo[34] = "NETC4";  valores_codigoativo[35] = "PCAR4";
    valores_codigoativo[36] = "SDIA4"; valores_codigoativo[37] = "RSID3";  valores_codigoativo[38] = "SBSP3";
    valores_codigoativo[39] = "TMCP4"; valores_codigoativo[40] = "TAMM4";  valores_codigoativo[41] = "TCSL4";
    valores_codigoativo[42] = "TRPL4"; valores_codigoativo[43] = "TLPP4";  valores_codigoativo[44] = "TNLP4";
    valores_codigoativo[45] = "PRGA3"; valores_codigoativo[46] = "UBBR11"; valores_codigoativo[47] = "UGPA4";
    valores_codigoativo[48] = "LIGT3"; valores_codigoativo[49] = "VIVO4";
*/
    svm = false;
    nb = false;
    bls = false; //always true for analysis purpose
    reglog = false;

    svm_validacao_cruzada1 = false; //k-dobras
    svm_validacao_cruzada2 = false; //sliding
    svm_validacao_cruzada3 = true; //80-20

    reglog_validacao_cruzada1 = false; //k-dobras
    reglog_validacao_cruzada2 = false; //sliding
    reglog_validacao_cruzada3 = true; //80-20

    nb_validacao_cruzada1 = false; //k-dobras
    nb_validacao_cruzada2 = false; //sliding
    nb_validacao_cruzada3 = true; //80-20

    _fechamento = 1 ;//fixo

    _maximo = 1;_minimo = 1;_abertura = 1 ; _quantidade = 1;
    _volume = 0;


    //atualizandos no loop
    _cod_ativo = "";
    _janela = 0;
    _serie_final_lenght = 0;

    _fat_ajuste = 0;
    _perc_fat_ajuste = 0.0;

    _dobras = 4;
    _tam_janela_desl = 140;
    _perc_treino_exp3 = 0.8;

    c = 0; soma_acuracia = 0;
    nextid = getNextId(); //getting the next execution id from database control

    _scaled_data = 1; //0 = nao, 1 = min-max, 2 = padronizacao

    _dt_inicio_periodo = "2006-01-01";_dt_fim_periodo = "2007-03-31";

    param.degree = 3; //set degree in kernel function (default 3)
    param.coef0 = 0; //set coef0 in kernel function (default 0) = K

    param.svm_type = 1; //0 = C-SVC, 1 = nu-SVC, 2 = one-class SVM, 3 = epsilon-SVR, 4 = nu-SVR (3 e 4 para regressao)
    param.kernel_type = 2; //0 = linear: u'*v / 1 = polynomial: (gamma*u'*v + coef0)^degree / 2 = radial basis function: exp(-gamma*|u-v|^2) / 3 = sigmoid: tanh(gamma*u'*v + coef0)
    //polinomial

    //polinomial, sigmoid e RBF
    param.gamma = 1; //set gamma in kernel function (default 1/num_features) *atualizado no loop
    //polinomial e sigmoid

    param.C = 1; //set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)
    param.nu = 0.5; //set the parameter nu of nu-SVC, one-class SVM, and nu-SVR (default 0.5)
            param.p = 0.1; //set the epsilon in loss function of epsilon-SVR (default 0.1)
            param.cache_size = 100; //set cache memory size in MB (default 100)
    param.eps = 1e-3; //set tolerance of termination criterion (default 0.001)
    param.shrinking = 1; //whether to use the shrinking heuristics, 0 or 1 (default 1)
    param.probability = 0; //whether to train a SVC or SVR model for probability estimates, 0 or 1 (default 0)
    param.nr_weight = 0; //set the parameter C of class i to weight*C, for C-SVC (default 1)
            param.weight_label = NULL;
            param.weight = NULL;

    //parametrizacoes SVM
    _param1_svm = param.svm_type;
    _param2_svm = param.kernel_type;
    _param3_svm = param.degree;

    _param5_svm = param.coef0;
    _param6_svm = param.C;
    _param7_svm = param.nu;
    _param8_svm = param.p;
    _param9_svm = param.cache_size;
    _param10_svm = param.eps;
    _param11_svm = param.shrinking;
    _param12_svm = param.probability;
    _param13_svm = param.nr_weight;

    //parametrizacoes Regressao Logistica
    _param1_rl = "0";
    _param2_rl = "0";
    _param3_rl = "0";



    /**************************** INICIO DO PROGRAMA *****************************/
    //separarValores = trim
    //dividirExemplos = separa cada linha do corpus como um exemplo (TRUE: não alterar)
    //se colocar dividirExemplos = TRUE e usar separados errado (ex a virgula, para pegar quantidade menor) entao da problema.
    //ele se perde na divisao de exemplos

    std::string s;
    //arquivo auxiliar para analise posterior
    ofstream outfileCVS;
    ofstream outfileK;

    try{

        classes.push_back("-1");
        classes.push_back("+1");

        outfileCVS.open("../outputs/saida.txt", fstream::out);
        outfileK.open("../outputs/saida_k.txt", fstream::out);

        outfileCVS.clear();
        outfileK.clear();

        outfileCVS << "ATIVO;JANELA;BLS;SVM;SVM_K;NB;NB_K;RL;RL_K" << endl;


        for (i=0; i<valores_codigoativo.size();i++){

            _out_bls = 0;
            _cod_ativo = valores_codigoativo[i];

            cout << "**********************************************" << endl ;
            cout << "Processando Ativo " << valores_codigoativo[i] << endl ;
            cout << "**********************************************" << endl ;
            outfileCVS << valores_codigoativo[i] << ";";

            outfileK << "**********************************************" << endl ;
            outfileK << "Processando Ativo " << valores_codigoativo[i] << endl ;
            outfileK << "**********************************************" << endl ;


            //realiza a execucao com distintos parametros de janelas j para o ativo i
            for (j=0; j<valores_janelas.size();j++){

                _out_nb = 0;
                _out_nb_exp1 = 0;
                _out_nb_exp2 = 0;
                _out_nb_exp3 = 0;
                _out_rl = 0;
                _out_rl_exp1 = 0;
                _out_rl_exp2 = 0;
                _out_rl_exp3 = 0;
                _out_svm = 0;
                _out_svm_exp1 = 0;
                _out_svm_exp2 = 0;
                _out_svm_exp3 = 0;


                _janela = valores_janelas[j];

                param.gamma = 1.0/_janela;
                //param.gamma = 0.0001;
                _param4_svm = param.gamma;



                CorpusMatriz objCorpus(vector<string>(), ';', true, true);
                atributosTreino.clear();

                objCorpus.carregarArquivo( "../inputs/bolsasp/ativos/ativos_" + valores_codigoativo[i] );

                janela_max = valores_janelas[valores_janelas.size() - 1]; //max value of window
                _total_dias_serie =  objCorpus.pegarQtdConjExemplos();
                _serie_final_lenght = objCorpus.pegarQtdConjExemplos() - janela_max;

                //informa maior janela a ser executada e ação a ser lida vazia, inicialmente
                ProcessadorSerieHistorica psh(valores_janelas[j], "valor");
                //cria atributos a serem utilizados como parametros (d-1, d-2, d-..n)
                psh.criarAtributosAuxiliares(objCorpus, 1, valores_janelas[j]);
                //seta os valores dos parametros d-1, d-2, d-n + y no corpus para o ativo em questao e zera as saidas
                if (!psh.processarCorpus(objCorpus, janela_max).size())
                    { cout << "psh.processarCorpus: Erro\n";}

                std::stringstream outsaidaarq2;
                outsaidaarq2 << "../outputs/#corpus_prescale_" << valores_codigoativo[i] << "_J_" << valores_janelas[j] << ".txt";
                objCorpus.gravarArquivo(outsaidaarq2.str());

//                , coldif=0;
//                for (coldif=1;coldif <= _janela ; coldif++){
//                    std::string saux;
//                    std::stringstream outaux;
//                    outaux << coldif;
//                    s = "d-" + outaux.str();
//                    idColDiferenca_i = objCorpus.pegarPosAtributo(s);
//
//                    psh.normalizacao_min_max(objCorpus, idColDiferenca_i , -1, +1);
//
//                }

                iResposta    = objCorpus.pegarPosAtributo("y");
                iSaidaBLS    = objCorpus.pegarPosAtributo("saida_bls");
                iSaidaNB     = objCorpus.pegarPosAtributo("saida_nb");
                iSaidaRegLog = objCorpus.pegarPosAtributo("saida_reglog");
                iSaidaSVM    = objCorpus.pegarPosAtributo("saida_svm");

                //log
                outfileCVS << valores_janelas[j] << ";";
                outfileK << "janela = d-" << valores_janelas[j] << ";" << endl;
                if ((j+1) == valores_janelas.size()){
                    outfileK << "" << endl;
                }

                /**************** Atualizando vetor de treino + normalizacao ****************/
                if (true){

                    int idColNorm = 0;
                    for (int d=1; d<=valores_janelas[j]; d++){
                        std::stringstream out;
                        out << "d-" << d;
                        atributosTreino.push_back(out.str());

                        idColNorm = objCorpus.pegarPosAtributo(out.str());
                        if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        }
                        else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }

                    }

                    //teste com valores base
                    if (_fechamento){
                        atributosTreino.push_back("valor");
                        idColNorm = objCorpus.pegarPosAtributo("valor");
                        if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        } else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }
                    if (_maximo){
                        atributosTreino.push_back("maximo");
                        idColNorm = objCorpus.pegarPosAtributo("maximo");
                        if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        } else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }
                    if (_minimo){
                        atributosTreino.push_back("minimo");
                        idColNorm = objCorpus.pegarPosAtributo("minimo");
                        if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        }else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }
                    if (_abertura){
                        atributosTreino.push_back("abertura");
                        idColNorm = objCorpus.pegarPosAtributo("abertura");
                        if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        } else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }
                    if (_volume){
                        atributosTreino.push_back("volume");
                        idColNorm = objCorpus.pegarPosAtributo("volume");
                          if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        } else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }
                    if (_quantidade){
                        atributosTreino.push_back("quantidade");
                        idColNorm = objCorpus.pegarPosAtributo("quantidade");
                          if (_scaled_data == 1){
                            psh.normalizacao_min_max(objCorpus, idColNorm , -1, +1);
                        } else if (_scaled_data == 2){
                            psh.normalizacao_padronizacao(objCorpus, idColNorm);
                        }
                    }

                    //atributosTreino.push_back("considerar");
                }

                std::stringstream outsaidaarq3;
                outsaidaarq3 << "../outputs/#corpus_posscale_" << valores_codigoativo[i] << "_J_" << valores_janelas[j] << ".txt";
                objCorpus.gravarArquivo(outsaidaarq3.str());

                cout << "    gerando modelos para janela -> d-" << valores_janelas[j] << endl ;

                //inicio_janela = valores_janelas[j] + 1;
                vector<float> tmpret;

                /********************************* LibSVM *********************************/
                if (svm) {

                    Classificador *objClassLibSvm;
                    LibSvm objLSVM(atributosTreino, classes, param);
                    objClassLibSvm = objLSVM.executarTreinamento(objCorpus, iResposta);
                    objClassLibSvm->executarClassificacao(objCorpus, iSaidaSVM);
                    ////cout << objClassLibSvm->descricaoConhecimento();
                    tmpret= objAvalAcur.calcularDesempenho(objCorpus, iResposta, iSaidaSVM);
                    _out_svm = tmpret[0];
                    _tot_hits_C0_svm = tmpret[3];
                    _tot_hits_C1_svm = tmpret[4];

                    cout << "    SVM [treino]: " << _out_svm * 100 << "%" << endl;

                    outfileCVS << _out_svm << ";";
                    //delete objClassLibSvm;
                    objLSVM.~Treinador();

                }
                // ******************************** TESTE DE EXPERIMENTO1 (k-dobras) ********************************
                if (svm_validacao_cruzada1){

                    LibSvm objLSVME1(atributosTreino, classes, param);
                    ValidadorKFoldDeslizante objValidadorSVM1(objAvalAcur, _dobras);
                    vector< vector < float > > vret1 = objValidadorSVM1.executarExperimento(objLSVME1, objCorpus, iResposta, iSaidaSVM);
                    objLSVME1.~Treinador();
                    objValidadorSVM1.~Validador();

                    cout << "**********************************************" << endl ;
                    c=0;soma_acuracia = 0;

                    outfileK << "Inicio dos Testes SVM - Experimento 1" << endl;

                    _tot_hits1_C0_svm = 0;
                    _tot_hits1_C1_svm = 0;
                    _tot_oc_test1_C0 = 0;
                    _tot_oc_test1_C1 = 0;

                    for (c=0;c<_dobras - 1;c++){
                        acuracia = 100 * vret1[c][0];
                        cout << c+1 << " execucao: " << acuracia << "%" << endl;
                        outfileK << c+1 << " execucao: " << vret1[c][0] << endl;
                        soma_acuracia += vret1[c][0];

                        _tot_oc_test1_C0 += vret1[c][1];
                        _tot_oc_test1_C1 += vret1[c][2];

                        _tot_hits1_C0_svm += vret1[c][3];
                        _tot_hits1_C1_svm += vret1[c][4];

                    }
                    _out_svm_exp1 = soma_acuracia/(_dobras - 1);

                    cout << " Validacao experimento 1 (Media SVM) " << _dobras << " dobras: " << 100 * _out_svm_exp1 << "%" << endl  ;
                    cout << "**********************************************" << endl ;
                    outfileCVS << _out_svm_exp1 << ";" ;
                    outfileK << "Media SVM_K_exp1: " << _dobras << "-dobras: " << _out_svm_exp1 << endl;
                }
                // ******************************** TESTE DE EXPERIMENTO2 (sliding) ********************************
                if (svm_validacao_cruzada2){

                    LibSvm objLSVME2(atributosTreino, classes, param);
                    ValidadorKFoldDeslizante objValidadorSVM2(objAvalAcur, 0);
                    vector< vector< float > > vret2 = objValidadorSVM2.executarExperimento2(objLSVME2, objCorpus, iResposta, iSaidaSVM, _tam_janela_desl);
                    objLSVME2.~Treinador();
                    objValidadorSVM2.~Validador();

                    int tot = objCorpus.pegarQtdSentencas() - _tam_janela_desl ;
                    cout << "**********************************************" << endl ;
                    c=0;soma_acuracia = 0;_records_used_adjustment_factor=0;
                    int contar=0;
                    outfileK << "Inicio dos Testes SVM - Experimento 2" << endl;

                    _tot_hits_C0_svm = 0;
                    _tot_hits_C1_svm = 0;
                    _tot_oc_test_C0 = 0;
                    _tot_oc_test_C1 = 0;

                    _exp2_fn_svm = 0;
                    _exp2_fp_svm = 0;


                    //float temp_tot_hits_c0_svm = 0, temp_tot_hits_c1_svm = 0;

                    tot=10;

                    for (c=0; c < tot  ;c++){
                        if (vret2[c][0] == -1){
                            //acuracia = 100.* vret22[c][0];
                            cout << c+1 << " execucao: nao considerado" << endl;
                            //cout << c+1 << " execucao:" << acuracia << "%" << endl;
                            //outfileK << c+1 << " execucao: " << acuracia << "%" << endl;
                        }
                        else
                        {

                             //temp_tot_hits_c0_svm += vret2[c][3];
                             //temp_tot_hits_c1_svm += vret2[c][4];
                            _tot_oc_test_C0 += vret2[c][1];
                            _tot_oc_test_C1 += vret2[c][2];

                             _tot_hits_C0_svm += vret2[c][3];
                             _tot_hits_C1_svm += vret2[c][4];

                             _exp2_fn_svm += vret2[c][5];
                             _exp2_fp_svm += vret2[c][6];

                             _records_used_adjustment_factor+=1;
                             cout << c+1 << " execucao:" << 100.* vret2[c][0] << "%" << endl;
                             outfileK << c+1 << " execucao:" << vret2[c][0] << endl;
                             soma_acuracia += vret2[c][0];
                             contar++;
                        }
                    }
                    if (soma_acuracia == 0) {
                        _out_svm_exp2 = 0;
                    }
                    else{
                        _out_svm_exp2 = soma_acuracia/contar;
                    }

                    //_tot_hits_C0_svm = ceil(temp_tot_hits_c0_svm / contar );
                    //_tot_hits_C1_svm = ceil(temp_tot_hits_c1_svm / contar);

                    cout << " Validacao experimento 2 (Media SVM) " << tot << " execucoes: " << _out_svm_exp2 * 100 << "%" << endl  ;
                    cout << "**********************************************" << endl ;
                    outfileCVS << _out_svm_exp2 << ";" ;
                    outfileK << "Media SVM_K_exp2: " << (soma_acuracia/tot) << ";" ;
                }
                // ******************************** TESTE DE EXPERIMENTO3 (80-20) ********************************
                if (svm_validacao_cruzada3){

                    LibSvm objLSVME3(atributosTreino, classes, param);
                    ValidadorKFoldDeslizante objValidadorSVM3(objAvalAcur, 0);
                    int tamanho_treino = floor((objCorpus.pegarQtdSentencas() * _perc_treino_exp3));
                    vector< vector< float > > vret3 = objValidadorSVM3.executarExperimento3(objLSVME3, objCorpus, iResposta, iSaidaSVM, tamanho_treino, valores_codigoativo[i], valores_janelas[j], "SVM");
                    objLSVME3.~Treinador();
                    objValidadorSVM3.~Validador();

                    _tot_hits3_C0_svm = 0;
                    _tot_hits3_C1_svm = 0;
                    _tot_oc_test3_C0 = 0;
                    _tot_oc_test3_C1 = 0;
                    _exp3_fn_svm = 0;
                    _exp3_fp_svm = 0;
                    _out_svm_exp3 = vret3[0][0];

                    _tot_oc_test3_C0 = vret3[0][1]; //mesmo valor para todos os algoritmos
                    _tot_oc_test3_C1 = vret3[0][2]; //mesmo valor para todos os algoritmos

                    _tot_hits3_C0_svm = vret3[0][3];
                    _tot_hits3_C1_svm = vret3[0][4];
                    _exp3_fn_svm = vret3[0][5];
                    _exp3_fp_svm = vret3[0][6];


                    cout << "**********************************************" << endl ;
                    cout << " Validacao experimento 3 (SVM): " << _out_svm_exp3 * 100 << "%" << endl ;
                    cout << "**********************************************" << endl ;
                    outfileCVS << (_out_svm_exp3) << ";" ;
                    outfileK << "Inicio dos Testes SVM - Experimento 3" << endl;
                    outfileK << "SVM_K_exp3: " << (_out_svm_exp3) << endl;

                }


                /********************************* Naive Bayes *********************************/
                if (nb) {
                    Classificador *objClass;
                    NaiveBayes objNB(atributosTreino, classes);
                    objClass = objNB.executarTreinamento(objCorpus, iResposta);
                    objClass->executarClassificacao(objCorpus, iSaidaNB);
                    //cout << objClass->descricaoConhecimento();
                    tmpret = objAvalAcur.calcularDesempenho(objCorpus, iResposta, iSaidaNB);
                    _out_nb = tmpret[0];
                    cout << "    NB  [treino]: " << _out_nb * 100. << "%"  << endl;
                    outfileCVS << _out_nb << ";";
                    delete objClass;
                }
                 // ******************************** TESTE DE EXPERIMENTO1 (k-dobras) ********************************
                if (nb_validacao_cruzada1){
                        _tot_hits1_C0_nb = 0;
                        _tot_hits1_C1_nb = 0;
                        ValidadorKFoldDeslizante objValidadorNB(objAvalAcur, _dobras);
                        NaiveBayes objNBExp1(atributosTreino, classes);
                        vector< vector< float > > v = objValidadorNB.executarExperimento(objNBExp1, objCorpus, iResposta, iSaidaNB);
                        float c,soma_acuracia = 0;
                        outfileK << "Inicio dos Testes NB - Experimento 1" << endl;

                        for (c=0;c<_dobras - 1;c++){
                            acuracia = 100 * v[c][0];
                            cout << c+1 << " validacao: - " << acuracia << "%" << endl;
                            outfileK << c+1 << " validacao: - " << v[c][0] << endl;
                            soma_acuracia += v[c][0];

                            _tot_hits1_C0_nb += v[c][3];
                            _tot_hits1_C1_nb += v[c][4];

                        }

                        _out_nb_exp1 = (soma_acuracia/(_dobras-1));

                        cout << " Validacao experimento 1 (Media NB) " << _dobras << " dobras: " << 100 *_out_nb_exp1 << "%" << endl  ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << _out_nb_exp1 << ";" ;
                        outfileK << " Media NB " << _dobras << "-dobras: " << _out_nb_exp1 << endl  ;
                }
                // ******************************** TESTE DE EXPERIMENTO2 (sliding) ********************************
                if (nb_validacao_cruzada2){


                        ValidadorKFoldDeslizante objValidadorNB2(objAvalAcur, 0);
                        NaiveBayes objNBExp2(atributosTreino, classes);
                        vector< vector< float > > vret2 = objValidadorNB2.executarExperimento2(objNBExp2, objCorpus, iResposta, iSaidaNB, _tam_janela_desl);
                        objNBExp2.~Treinador();
                        objValidadorNB2.~Validador();

                        int tot = objCorpus.pegarQtdSentencas() - _tam_janela_desl ;
                        cout << "**********************************************" << endl ;
                        c=0;soma_acuracia = 0;
                        int contar=0;
                        outfileK << "Inicio dos Testes NB - Experimento 2" << endl;

                        _tot_hits_C0_nb = 0;
                        _tot_hits_C1_nb = 0;
                        _exp2_fn_nb = 0;
                        _exp2_fp_nb = 0;

                    tot=10;

                        for (c=0; c < tot  ;c++){
                            if (vret2[c][0] == -1){
                                //acuracia = 100.* vret22[c][0];
                                cout << c+1 << " execucao: nao considerado" << endl;
                                //cout << c+1 << " execucao:" << acuracia << "%" << endl;
                                //outfileK << c+1 << " execucao: " << acuracia << "%" << endl;
                            }
                            else
                            {
//                                _tot_oc_test_C0 += vret2[c][1]; //se sempre rodar o SVM nao precisar chamar este aqui...mas...
//                                _tot_oc_test_C1 += vret2[c][2];

                                _tot_hits_C0_nb += vret2[c][3];
                                _tot_hits_C1_nb += vret2[c][4];
                                _exp2_fn_nb += vret2[c][5];
                                _exp2_fp_nb += vret2[c][6];

                                 cout << c+1 << " execucao:" << 100 * vret2[c][0] << "%" << endl;
                                 outfileK << c+1 << " execucao:" << vret2[c][0] << endl;
                                 soma_acuracia += vret2[c][0];
                                 contar++;
                            }
                        }
                         if (soma_acuracia == 0) {
                             _out_nb_exp2=0;
                         }
                         else{
                            _out_nb_exp2 = (soma_acuracia/(contar));
                        }

                        cout << " Validacao experimento 2 (Media NB) " << tot << " execucoes: " << 100 *(_out_nb_exp2) << "%" << endl  ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << _out_nb_exp2 << ";" ;
                        outfileK << "Media NB_K_exp2: " << _out_nb_exp2 << ";" ;
                }
                // ******************************** TESTE DE EXPERIMENTO3 (80-20) ********************************
                if (nb_validacao_cruzada3){
                        _tot_hits3_C0_nb = 0;
                        _tot_hits3_C1_nb = 0;
                        _exp3_fn_nb = 0;
                        _exp3_fp_nb = 0;

                        ValidadorKFoldDeslizante objValidadorNB3(objAvalAcur, 0);
                        int tamanho_treino = floor((objCorpus.pegarQtdSentencas() * _perc_treino_exp3)); //80% da base para treino
                        NaiveBayes objNBExp3(atributosTreino, classes);
                        vector< vector< float > > vret3 = objValidadorNB3.executarExperimento3(objNBExp3, objCorpus, iResposta, iSaidaNB, tamanho_treino,valores_codigoativo[i], valores_janelas[j], "NB");

                        //_tot_oc_test3_C0 -> calculado no SVM, mesmo para todos os algoritmos
                        //_tot_oc_test3_C1 -> calculado no SVM, mesmo para todos os algoritmos
                        _tot_hits3_C0_nb = vret3[0][3];
                        _tot_hits3_C1_nb = vret3[0][4];

                        _out_nb_exp3 = vret3[0][0];

                        _exp3_fn_nb = vret3[0][5];
                        _exp3_fp_nb = vret3[0][6];



                        cout << "**********************************************" << endl ;
                        cout << " Validacao experimento 3 (NB): " << 100 * (_out_nb_exp3) << "%" << endl ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << (_out_nb_exp3) << ";" ;
                        outfileK << "Inicio dos Testes NB - Experimento 3" << endl;
                        outfileK << "NB_K_exp3: " << (_out_nb_exp3) << endl;
                }


                /********************************* Regressão Logística *********************************/
                if (reglog) {

                    Classificador *objClassRegLog;
                    RegressaoLogistica objRL(atributosTreino,classes);
                    objClassRegLog = objRL.executarTreinamento(objCorpus, iResposta);
                    objClassRegLog->executarClassificacao(objCorpus, iSaidaRegLog);
                    tmpret = objAvalAcur.calcularDesempenho(objCorpus, iResposta, iSaidaRegLog);
                    _out_rl = tmpret[0];


                    cout << "    RL  [treino]: " << _out_rl*100. << "%"  << endl;

                    outfileCVS << _out_rl << ";";
                    delete objClassRegLog;

                }
                // ******************************** TESTE DE EXPERIMENTO1 ********************************
                if (reglog_validacao_cruzada1){
                        c = 0;

                        ValidadorKFoldDeslizante objValidadorRL(objAvalAcur, _dobras);
                        RegressaoLogistica objRLExp1(atributosTreino,classes);
                        vector< vector< float > > v = objValidadorRL.executarExperimento(objRLExp1, objCorpus, iResposta, iSaidaRegLog);
                        float c,soma_acuracia = 0;
                        outfileK << "Inicio dos Testes RL - Experimento 1" << endl;

                        _tot_hits1_C0_rl=0;
                        _tot_hits1_C1_rl=0;

                        for (c=0;c<_dobras - 1;c++){
                            cout << c+1 << " validacao: - " << 100.* v[c][0] << "%" << endl;
                            outfileK << c+1 << " validacao: - " <<  v[c][0] << endl;
                            soma_acuracia += v[c][0];

                            _tot_hits1_C0_rl += v[c][3];
                            _tot_hits1_C1_rl += v[c][4];
                        }

                        _out_rl_exp1 = (soma_acuracia/(_dobras - 1));

                        cout << " Validacao experimento 1 (Media RL) " << _dobras << " dobras: " << 100.*(_out_rl_exp1) << "%" << endl  ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << _out_rl_exp1 << ";" ;
                        outfileK << " Media RL " << _dobras << "-dobras: " << _out_rl_exp1 << endl  ;
                }
                // ******************************** TESTE DE EXPERIMENTO2 ********************************
                if (reglog_validacao_cruzada2){

                        ValidadorKFoldDeslizante objValidadorRL2(objAvalAcur, 0);
                        RegressaoLogistica objRLExp2(atributosTreino,classes);
                        vector< vector< float > > vret2 = objValidadorRL2.executarExperimento2(objRLExp2, objCorpus, iResposta, iSaidaRegLog, _tam_janela_desl);
                        objRLExp2.~Treinador();
                        objValidadorRL2.~Validador();

                        int tot = objCorpus.pegarQtdSentencas() - _tam_janela_desl ;
                        cout << "**********************************************" << endl ;
                        c=0;soma_acuracia = 0;
                        int contar=0;
                        outfileK << "Inicio dos Testes RL - Experimento 2" << endl;

                        _tot_hits_C0_rl = 0;
                        _tot_hits_C1_rl = 0;
                        _exp2_fn_rl = 0;
                        _exp2_fp_rl = 0;

                        for (c=0; c < tot  ;c++){
                            if (vret2[c][0] == -1){
                                //acuracia = 100.* vret22[c][0];
                                cout << c+1 << " execucao: nao considerado" << endl;
                                //cout << c+1 << " execucao:" << acuracia << "%" << endl;
                                //outfileK << c+1 << " execucao: " << acuracia << "%" << endl;
                            }
                            else
                            {
//                                _tot_oc_test_C0 += vret2[c][1]; //se sempre rodar o SVM nao precisar chamar este aqui...mas...
//                                _tot_oc_test_C1 += vret2[c][2];
//
                                _tot_hits_C0_rl += vret2[c][3];
                                _tot_hits_C1_rl += vret2[c][4];
                                _exp2_fn_rl += vret2[c][5];
                                _exp2_fp_rl += vret2[c][6];

                                 //acuracia = 100.* vret2[c][0];
                                 cout << c+1 << " execucao:" << 100.* vret2[c][0] << "%" << endl;
                                 outfileK << c+1 << " execucao:" << vret2[c][0] << endl;
                                 soma_acuracia += vret2[c][0];
                                 contar++;
                            }
                        }
                          if (soma_acuracia == 0) {
                              _out_rl_exp2=0;
                          }
                          else{
                            _out_rl_exp2 = (soma_acuracia/(contar));
                        }

                        cout << " Validacao experimento 2 (Media RL) " << tot << " execucoes: " << 100.*(_out_rl_exp2) << "%" << endl  ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << _out_rl_exp2 << ";" ;
                        outfileK << "Media RL_K_exp2: " << _out_rl_exp2 << ";" ;

                }
                // ******************************** TESTE DE EXPERIMENTO3 ********************************
                if (reglog_validacao_cruzada3){

                        ValidadorKFoldDeslizante objValidadorRL3(objAvalAcur, 0);
                        int tamanho_treino = floor((objCorpus.pegarQtdSentencas() * _perc_treino_exp3)); //80% da base para treino
                        RegressaoLogistica objRLExp3(atributosTreino,classes);
                        vector< vector< float > > vret3 = objValidadorRL3.executarExperimento3(objRLExp3, objCorpus, iResposta, iSaidaRegLog, tamanho_treino,valores_codigoativo[i], valores_janelas[j],"RL");

                        _tot_hits3_C0_rl = 0;
                        _tot_hits3_C1_rl = 0;
                        _exp3_fn_rl = 0;
                        _exp3_fp_rl = 0;

                        _out_rl_exp3 = vret3[0][0];
                        //_tot_oc_test3_C0 -> calculado no SVM, mesmo para todos os algoritmos
                        //_tot_oc_test3_C1 -> calculado no SVM, mesmo para todos os algoritmos
                        _tot_hits3_C0_rl = vret3[0][3];
                        _tot_hits3_C1_rl = vret3[0][4];
                        _exp3_fn_rl = vret3[0][5];
                        _exp3_fp_rl = vret3[0][6];

                        cout << "**********************************************" << endl ;
                        cout << " Validacao experimento 3 (RL): " << _out_rl_exp3 * 100 << "%" << endl ;
                        cout << "**********************************************" << endl ;
                        outfileCVS << (_out_rl_exp3) << ";" ;
                        outfileK << "Inicio dos Testes RL - Experimento 3" << endl;
                        outfileK << "RL_K_exp3: " << (_out_rl_exp3) << endl;

                    }


                /** Base Line System **/
                //sempre rodar o BaseLine para pegar total de ocorrencias de cada classe.
                if (true) {
                    if (j==0){
                        ClassificadorBLS *cbls;
                        cbls = new ClassificadorBLS(classes);
                        cbls->executarClassificacao(objCorpus, iSaidaBLS);
                        //cout << cbls->descricaoConheciatributo_atualmento();

                        _tot_oc_C0 = 0;
                        _tot_oc_C1 = 0;

                        tmpret = objAvalAcur.calcularDesempenho(objCorpus, iResposta, iSaidaBLS);
                        _out_bls = tmpret[0];

                        _tot_oc_C0 = tmpret[1];
                        _tot_oc_C1 = tmpret[2];


                        cout << "    BLS         : " << _out_bls * 100. << "%"  << endl;
                        outfileCVS << _out_bls << ";";
                        delete cbls;

                    } //se nao for primeira vez, pega memoria
                    else{
                        cout << "    BLS         : " << _out_bls * 100. << "%"  ;
                        outfileCVS << _out_bls << ";";
                    }

                }
                else{
                    outfileCVS << "0;";
                }

                cout << endl;

                std::stringstream outsaidaarq;
                outsaidaarq << "../outputs/#corpus_final_" << valores_codigoativo[i] << "_J_" << valores_janelas[j] << ".txt";
                objCorpus.gravarArquivo(outsaidaarq.str());

                /**************************************************************
                gravando no banco de dados
                ***************************************************************/
                if (true){
                // grava resultados no banco de dados
                insertOnDB(_janela,                     //defasagem
                           _cod_ativo,                  //codigo do ativo financeiro
                           _out_bls,                    //acurácia BLS
                           _out_svm,                    //acurácia SVM
                           _out_svm_exp1,               //acurácia SVM - Validacao Exp1
                           _out_svm_exp2,               //acurácia SVM - Validacao Exp2
                           _out_svm_exp3,               //acurácia SVM - Validacao Exp3
                           _out_nb,                     //acurácia NB
                           _out_nb_exp1,                //acurácia NB - Validacao Exp1
                           _out_nb_exp2,                //acurácia NB - Validacao Exp2
                           _out_nb_exp3,                //acurácia NB - Validacao Exp3
                           _out_rl,                     //acurácia RL
                           _out_rl_exp1,                //acurácia RL - Validacao Exp1
                           _out_rl_exp2,                //acurácia RL - Validacao Exp2
                           _out_rl_exp3,                //acurácia RL - Validacao Exp3
                           _dobras,                     //dobras (usada no exp1)
                           _tam_janela_desl,            //tam janela desl (usada no exp2)
                           _perc_treino_exp3,           //% de registros treino (usada no exp3)
                           _maximo,                     //input - valor maximo do ativo no dia
                           _minimo,                     //input - valor minimo do ativo no dia
                           _fechamento,                 //input - valor fechamento do ativo no dia
                           _abertura,                   //input - valor abertura do ativo no dia
                           _volume,                     //input - volume de operacoes do ativo no dia
                           _fat_ajuste,                 //fator de corte para modelo
                           _perc_fat_ajuste,            //% do fator de corte para modelo
                           _param1_rl,                  //param RL
                           _param2_rl,                  //param RL
                           _param3_rl,                  //param RL
                           _param1_svm,                 //param SVM
                           _param2_svm,                 //param SVM
                           _param3_svm,                 //param SVM
                           _param4_svm,                 //param SVM
                           _param5_svm,                 //param SVM
                           _param6_svm,                 //param SVM
                           _param7_svm,                 //param SVM
                           _param8_svm,                 //param SVM
                           _param9_svm,                 //param SVM
                           _param10_svm,                //param SVM
                           _param11_svm,                //param SVM
                           _param12_svm,                //param SVM
                           _param13_svm,                //param SVM
                           _total_dias_serie,           //tamanho do conjunto
                           _dt_inicio_periodo,          //dt inicio da serie
                           _dt_fim_periodo,             //dt fim da serie
                           nextid,                      //sistema
                           _records_used_adjustment_factor,//total de registros considerados com fator de ajuste
                           _serie_final_lenght,         //tamanho do conjunto apos preprocessamento
                           _scaled_data,                //dados normalizados?
                           _tot_oc_C0,                  //total de ocorrencias da classe 0 no conjunto (treino)
                           _tot_oc_C1,                  //total de ocorrencias da classe 1 no conjunto (treino)
                           _tot_hits_C0_svm,            //total de acertos da classe 0 pelo SVM (exp 2)
                           _tot_hits_C1_svm,            //total de acertos da classe 1 pelo SVM (exp 2)
                           _tot_hits_C0_rl,             //total de acertos da classe 0 pela RL (exp 2)
                           _tot_hits_C1_rl,             //total de acertos da classe 1 pela RL (exp 2)
                           _tot_hits_C0_nb,             //total de acertos da classe 0 pelo NB (exp 2)
                           _tot_hits_C1_nb,             //total de acertos da classe 1 pelo NB (exp 2)
                           _tot_oc_test_C0,             //total de ocorrencias da classe 0 (exp 2)
                           _tot_oc_test_C1,             //total de ocorrencias da classe 1 (exp 2)
                           _tot_hits1_C0_svm,           //total de acertos da classe 0 pelo SVM (exp 1)
                           _tot_hits1_C1_svm,           //total de acertos da classe 1 pelo SVM (exp 1)
                           _tot_hits1_C0_rl,            //total de acertos da classe 0 pelo RL (exp 1)
                           _tot_hits1_C1_rl,            //total de acertos da classe 1 pelo RL (exp 1)
                           _tot_hits1_C0_nb,            //total de acertos da classe 0 pelo NB (exp 1)
                           _tot_hits1_C1_nb,            //total de acertos da classe 1 pelo NB (exp 1)
                           _tot_oc_test1_C0,            //total de ocorrencias da classe 0 (exp 1)
                           _tot_oc_test1_C1,            //total de ocorrencias da classe 1 (exp 1)
                           _tot_hits3_C0_svm,           //total de acertos da classe 0 pelo SVM (exp 3)
                           _tot_hits3_C1_svm,           //total de acertos da classe 1 pelo SVM (exp 3)
                           _tot_hits3_C0_rl,            //total de acertos da classe 0 pelo RL (exp 3)
                           _tot_hits3_C1_rl,            //total de acertos da classe 1 pelo RL (exp 3)
                           _tot_hits3_C0_nb,            //total de acertos da classe 0 pelo NB (exp 3)
                           _tot_hits3_C1_nb,            //total de acertos da classe 1 pelo NB (exp 3)
                           _tot_oc_test3_C0,            //total de ocorrencias da classe 0 (exp 3)
                           _tot_oc_test3_C1,           //total de ocorrencias da classe 1 (exp 3)
                           _quantidade,                //input - quantidade de operacoes do ativo no dia
                           _exp2_fn_svm,
                           _exp2_fp_svm,
                           _exp2_fn_nb,
                           _exp2_fp_nb,
                           _exp2_fn_rl,
                           _exp2_fp_rl,
                           _exp3_fn_svm,
                           _exp3_fp_svm,
                           _exp3_fn_nb,
                           _exp3_fp_nb,
                           _exp3_fn_rl,
                           _exp3_fp_rl
                           );


                        }

            } //fim j - janela
            cout << endl;
            outfileCVS << ";" << endl ;

        } //fim i - codigo ativo

        outfileCVS.flush();
        outfileCVS.close();
        outfileK.flush();
        outfileK.close();
        cout << "ID_EXECUTION_GROUP = " << nextid;
        cout << "Fim da execucao dos algoritmos. Pressione ENTER" ;

        cout << '\a';

        getchar();
        return 0;

        }catch(string err){
            if (outfileCVS.is_open())
            {
                outfileCVS.close();
            }
            if (outfileK.is_open())
            {
                outfileK.close();
            }
            cout << err;
            return 1;
        }

}







