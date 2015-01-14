# ime-esteves-master-2014
This is the main code for my master dissertation (C++). 

## Title
**_Predicao de tendencia de ativos em series financeiras utilizando algoritmos de aprendizado de maquina_**

## Abstract
Predict the behavior of nancial assets is a challenge that has been studied with
various techniques in recent years, either regression or classication models . Despite the
large amount of academic research, anticipate the movement of the nal price of an asset
is still a challenge to be overcome . The main diculty is nonlinear and non-stationary
behavior of nancial time series, as well as all uncertainty and noise found in the nancial
market.

This work focuses on nding stock price trends in the Brazilian market in certain
periods of time and covers the study of the behavior of three machine learning algorithms,
Naive Bayes (NB), Logistic Regression (LR) and Support Vector Machines (SVM) applied
to historical data for nancial time series. The behavior was compared with a basic
algorithm (Baseline - BLS) developed as a lower limit of quality for models generated.
As shown in similar studies in the area, the SVM algorithm outperforms the others
algorithms compared in this study. A detailed analysis shows that the parameter setting
has benets for the outcome of the models, overcoming with up to 32% . The LIBSVM
library has been integrated into the **FAMA framework**, both available as opensource
distribution.

In order to obtain models with larger capacity of generalization, the default behavior
of the cross-validation method has been changed to adjust the characteristics of the
problem, making it slide on time's period. It was shown that retrain the model in a
given period of time positively improves the hit rate when compared to the standard
cross-validation model.

## Setup the environment
Please set up the envirionment before use the code. See below required steps

1. Install local or remote MSSQL Server 2008 instance and run the script model file
2. Download Stock Exchange files containing the historical serie (there is a wrapper for **BOVESPA [Brazil]** already)
3. Change the algorithm parameters
4. Run the project experiments (testSH.cbp)
5. See the output files at /output folder

## The thesis can be downloaded at:
http://www.comp.ime.eb.br/dissertacoes/2014-Diego_Esteves.pdf
