#include "Data.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

using namespace std;

typedef struct Solucao
{

    vector<int> sequence;
    double valorObj;

}Solucao;

typedef struct InsertionInfo
{
    int noInserido;
    int arestaRemovida;
    double custo;   

}InsertionInfo;

void exibirSolucao(Solucao *s)
{
    for(int i = 0; i < s->sequence.size() - 1; i++)
    {
        cout << s->sequence[i] << " -> ";
    }
    cout << s->sequence.back() << endl;
}

void calcularValorObj(Solucao s, Data &data)
{
    s.valorObj = 0.0;
    for(int i = 0; i < s.sequence.size() - 1; i++)
    {
        s.valorObj += data.getDistance(s.sequence[i], s.sequence[i+1]);
    }
    cout << "Custo de S: " << s.valorObj << endl;
}

vector<InsertionInfo> calcularCustoInsercao(Solucao& s, vector<int>& CL, Data &data)
{
    vector<InsertionInfo> custoInsercao((s.sequence.size()-1)*CL.size());

    int l = 0;
    for(int a = 0; a < s.sequence.size() - 1; a++)
    {
        int i = s.sequence[a];
        int j = s.sequence[a+1];

        for(auto k : CL)
        {
            custoInsercao[l].custo = data.getDistance(i, k) + data.getDistance(k, j) - data.getDistance(i, j);
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    } 
    return custoInsercao;
}


vector<int> escolher3NosAleatorios(int dim)
{
    vector<int> nosEscolhidos(4);
    int nos[3];
    bool mesmo;

    for(int i = 0; i < 3; i++)
    {
         
        nos[i] = rand() % dim + 1;
        mesmo = 1;
        while(mesmo)
        {
            mesmo = 0;
            
        
            for(int j = 0; j < i; j++)
            {
                if(nos[i] == nos[j])
                {
                    nos[i] = rand() % dim + 1;
                    mesmo = 1;
                    break;
                }
            }
        }
    }

    nosEscolhidos[0] = nos[0];
    nosEscolhidos[1] = nos[1];
    nosEscolhidos[2] = nos[2];
    nosEscolhidos[3] = nos[0];


    

    return nosEscolhidos;
}

void InserirNaSolucao(Solucao &s, InsertionInfo &k, vector<int>& CL)
{
    s.sequence.insert(s.sequence.begin() + k.arestaRemovida + 1, k.noInserido);
    s.valorObj += k.custo;
    for(auto a  =  CL.begin(); a != CL.end(); a++)
    {
        if(*a == k.noInserido)
        {
            CL.erase(a);
            break;
        }
    }
}

vector<int> restoDosNos(vector<int> sequence, int dim)
{
    vector<int> CL(dim - (sequence.size() - 1));
    int index = 0;
    for(int i = 1; i <= dim; i++)
    {
        if(i != sequence[0] && i != sequence[1] && i != sequence[2])
        {
            CL[index++] = i;
        }
    }
    return CL;
}


bool caroSouN(InsertionInfo a, InsertionInfo b)
{
    return (a.custo < b.custo);
}

Solucao Construcao(Data &data, int &dim)
{
    double alpha;
    int selecionado, sla;
    Solucao s = {{}, 0.0};
    s.sequence = escolher3NosAleatorios(dim);
    vector <int> CL = restoDosNos(s.sequence, dim);
    while(!CL.empty())
    {
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL, data);
        sort(custoInsercao.begin(), custoInsercao.end(), caroSouN);
        alpha = (double) (rand() % RAND_MAX + 1)/ RAND_MAX;
        alpha += 0.0000000001;
        sla =  (int) (alpha * custoInsercao.size());
        if(sla == 0)
        {
            selecionado = 0;
        }
        else
        {
            selecionado = rand() % sla;
        }
        
        InserirNaSolucao(s, custoInsercao[selecionado], CL);
    }
    return s;
}

bool bestImprovementSwap(Solucao *s, Data &data)
{
    double bestDelta = 0;
    int best_i, best_j;
    double delta;

    for(int i = 1; i < s->sequence.size() -  1; i++)
    {
        int vi = s->sequence[i];
        int vi_next = s->sequence[i+1];
        int vi_prev = s->sequence[i-1];
        
        for(int j = i + 1; j < s->sequence.size()-1; j++)
        {
            int vj = s->sequence[j];
            int vj_next = s->sequence[j+1];
            int vj_prev = s->sequence[j-1];

            if(j > i)
            {
                delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi, vi_next) - data.getDistance(vj_prev, vj)
                        - data.getDistance(vj, vj_next) + data.getDistance(vi_prev, vj) + data.getDistance(vj, vi_next)
                        + data.getDistance(vj_prev, vi) + data.getDistance(vi, vj_next);
            }
            else if(j - i == 1 || i - j == 1)
            {
                delta = -data.getDistance(vi_prev, vi) - data.getDistance(vj, vj_next) + data.getDistance(vi_prev, vj)
                        +data.getDistance(vi, vj_next);
            }
            else if(i == j)
            {
                delta = 0;
                continue;
            }
            
            if(delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }   

    if(bestDelta < 0)
    {
        swap(s->sequence[best_i], s->sequence[best_j]);
        s->valorObj = s->valorObj + bestDelta;
        return true;
    }
    return false;
}

bool bestImprovement2Opt(Solucao *s, Data &data)
{

    double bestDelta = 0;
    int best_i, best_j;
    double delta;

    for(int i = 1; i < s->sequence.size() - 2; i++)
    {
        int vi = s->sequence[i];
        int vi_next = s->sequence[i+1];

        for(int j = i + 2; j < s->sequence.size() -1; j++)
        {
            int vj = s->sequence[j];
            int vj_next = s->sequence[j+1];

            if(i == 0 || j == s->sequence.size() - 1)
            {
                continue;
            }


            delta =  -data.getDistance(vi, vi_next) - data.getDistance(vj, vj_next) 
                     + data.getDistance(vj, vi_next) + data.getDistance(vi, vj_next);


            if(delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(bestDelta < 0)
    {
        reverse(s->sequence.begin() + best_i, s->sequence.begin() + best_j);
        s->valorObj = s->valorObj + bestDelta;
        return true;
    }
    return false;
}

bool bestImprovementOrOpt(Solucao *s, Data &data, int n)
{

    double bestDelta = 0;
    int best_i, best_j;
    double delta;

    switch (n)
    {
        case 1: //REINSERTION
            for (int i = 1; i < s->sequence.size(); i++) {
                int vi = s->sequence[i];
                int vi_prev = s->sequence[i - 1];
                int vi_next = s->sequence[i + 1];

                for (int j = 1; j < s->sequence.size(); j++) {
                    int vj = s->sequence[j];
                    int vj_next = s->sequence[j + 1];
                    int vj_prev = s->sequence[j - 1];

                    if (j < i) {
                        delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi, vi_next) -
                                data.getDistance(vj_prev, vj)
                                + data.getDistance(vi_prev, vi_next) + data.getDistance(vi, vj) +
                                data.getDistance(vj_prev, vi);
                    } else if (j > i) {
                        delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi, vi_next) -
                                data.getDistance(vj, vj_next)
                                + data.getDistance(vi_prev, vi_next) + data.getDistance(vi, vj) +
                                data.getDistance(vi, vj_next);
                    } else if (i == j - 1) {

                        delta = -data.getDistance(vi_prev, vi) - data.getDistance(vj, vj_next) +
                                data.getDistance(vi, vj)
                                + data.getDistance(vi_prev, vj) + data.getDistance(vi, vj_next);
                    }

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if (delta < 0) {
                if (best_i > best_j) {
                    s->sequence.insert(s->sequence.begin() + best_j, s->sequence[best_i]);
                    s->sequence.erase(s->sequence.begin() + best_i + 1);
                } else {
                    s->sequence.insert(s->sequence.begin() + best_j + 1, s->sequence[best_i]);
                    s->sequence.erase(s->sequence.begin() + best_i);
                }
                s->valorObj += bestDelta;
                return true;
            }
            return false;

        case 2: //OR-OPT-2
            for (int i = 1; i < s->sequence.size() - 2; i++) {
                int vi = s->sequence[i];
                int vi_next = s->sequence[i + 1];
                int vi_next2 = s->sequence[i + 2];
                int vi_prev = s->sequence[i - 1];

                for (int j = 1; j < s->sequence.size() - 1; j++) {
                    int vj = s->sequence[j];
                    int vj_next = s->sequence[j + 1];
                    int vj_next2 = s->sequence[j + 2];
                    int vj_prev = s->sequence[j - 1];

                    if (j > i) {
                        delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi_next, vi_next2) -
                                data.getDistance(vj_prev, vj)
                                - data.getDistance(vj_next, vj_next2) + data.getDistance(vi_prev, vj) +
                                data.getDistance(vj_next, vi_next2)
                                + data.getDistance(vj_prev, vi) + data.getDistance(vi_next, vj_next2);
                    } else if (j < i) {
                        delta = -data.getDistance(vj_prev, vj) - data.getDistance(vj_next, vj_next2) -
                                data.getDistance(vi_prev, vi)
                                - data.getDistance(vi_next, vi_next2) + data.getDistance(vi_prev, vj) +
                                data.getDistance(vj_next, vi_next2)
                                + data.getDistance(vj_prev, vi) + data.getDistance(vi_next, vj_next2);
                    } else if (i == j || i == j - 1 || i == j + 1) {
                        delta = 0;
                        continue;
                    }

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if (bestDelta < 0) {
                if (best_i > best_j) {
                    s->sequence.insert(s->sequence.begin() + best_j + 1, s->sequence[best_i]);
                    s->sequence.insert(s->sequence.begin() + best_j + 2, s->sequence[best_i + 1]);
                    s->sequence.erase(s->sequence.begin() + best_i);
                    s->sequence.erase(s->sequence.begin() + best_i + 1);
                } else {
                    s->sequence.insert(s->sequence.begin() + best_j, s->sequence[best_i]);
                    s->sequence.insert(s->sequence.begin() + best_j + 1, s->sequence[best_i + 2]);
                    s->sequence.erase(s->sequence.begin() + best_i);
                    s->sequence.erase(s->sequence.begin() + best_i + 2);
                }
                s->valorObj += bestDelta;
                return true;
            }
            return false;

        case 3: //OR-OPT-3
            for (int i = 1; i < s->sequence.size() - 3; i++) {
                int vi = s->sequence[i];
                int vi_next2 = s->sequence[i + 2];
                int vi_next3 = s->sequence[i + 3];
                int vi_prev = s->sequence[i - 1];

                for (int j = 1; j < s->sequence.size() - 2; j++) {
                    int vj = s->sequence[j];
                    int vj_next2 = s->sequence[j + 2];
                    int vj_next3 = s->sequence[j + 3];
                    int vj_prev = s->sequence[j - 1];

                    if (j > i) {
                        delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi_next2, vi_next3) -
                                data.getDistance(vj_prev, vj)
                                - data.getDistance(vj_next2, vj_next3) + data.getDistance(vi_prev, vj) +
                                data.getDistance(vj_next2, vi_next3)
                                + data.getDistance(vj_prev, vi) + data.getDistance(vi_next2, vj_next3);
                    } else if (j < i) {

                        delta = -data.getDistance(vj_prev, vj) - data.getDistance(vj_next2, vj_next3) -
                                data.getDistance(vi_prev, vi)
                                - data.getDistance(vi_next2, vi_next3) + data.getDistance(vi_prev, vj) +
                                data.getDistance(vj_next2, vi_next3)
                                + data.getDistance(vj_prev, vi) + data.getDistance(vi_next2, vj_next3);
                    } else if (i == j || i == j - 2 || i == j + 2) {
                        delta = 0;
                        continue;
                    }

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if (bestDelta < 0) {
                if (best_i > best_j) {
                    s->sequence.insert(s->sequence.begin() + best_j + 1, s->sequence[best_i]);
                    s->sequence.insert(s->sequence.begin() + best_j + 2, s->sequence[best_i + 1]);
                    s->sequence.insert(s->sequence.begin() + best_j + 3, s->sequence[best_i + 2]);
                    s->sequence.erase(s->sequence.begin() + best_i);
                    s->sequence.erase(s->sequence.begin() + best_i + 1);
                    s->sequence.erase(s->sequence.begin() + best_i + 2);
                } else {
                    s->sequence.insert(s->sequence.begin() + best_j, s->sequence[best_i + 1]);
                    s->sequence.insert(s->sequence.begin() + best_j + 1, s->sequence[best_i + 2]);
                    s->sequence.insert(s->sequence.begin() + best_j + 2, s->sequence[best_i + 3]);
                    s->sequence.erase(s->sequence.begin() + best_i + 1);
                    s->sequence.erase(s->sequence.begin() + best_i + 2);
                    s->sequence.erase(s->sequence.begin() + best_i + 3);
                }
                s->valorObj += bestDelta;
                return true;
            }
            return false;
    }
    return false;
}

void BuscaLocal(Solucao *s, Data &data)
{
    vector<int> NL = {1, 2, 3 , 4, 5};  
    bool improved = false;
    
    while(NL.empty() == false)
    {
        int n = rand() % NL.size();

        switch (NL[n])
        {
            case 1:
                improved = bestImprovementSwap(s, data);
                break;
            case 2:
                improved = bestImprovement2Opt(s, data);
                break;
            case 3:
                improved = bestImprovementOrOpt(s, data, 1);
                break;
            case 4:
                improved = bestImprovementOrOpt(s, data, 2);
                break;
            case 5:
            {
                improved = bestImprovementOrOpt(s, data, 3);
                break;
            }
        }
        if(improved)
        {
            NL = {1, 2, 3 , 4, 5};
        }
        else
        {
            NL.erase(NL.begin() + n);
        }
    }
}

//Solucao Pertubacao(Solucao s, Data &data)
//{
  //  Solucao pert = s;

//    int n = pert.sequence.size();
  //  int t1 = 2;
    //int t2 = (n - 1)/10;


    //return pert;
//}


Solucao ILS(int maxIter, int maxIterIls,int n,  Data &data)
{
    Solucao bestOfAll;

    bestOfAll.valorObj = INFINITY;

    for(int i = 0; i < maxIter; i++)
    {
        Solucao s = Construcao(data, n);
        Solucao best = s;

        int iterILS = 0;

        while (iterILS <= maxIterIls)
        {
            BuscaLocal(&s, data);
            if(s.valorObj < best.valorObj)
            {
                best = s;
                iterILS = 0;
            }
            //s = Perturbacao(best, data);
            iterILS++;
        }

        if(best.valorObj < bestOfAll.valorObj)
        {
            bestOfAll = best;
        }
    }
    return bestOfAll;
}

int main(int argc, char** argv) 
{

    Solucao best;
    int maxIter = 50;
    int maxIterIls;
    double cto = 0.0;
    auto data = Data(argc, argv[1]);
    data.read();
    int n = data.getDimension();

    if(n >= 150)
    {
        maxIterIls = n/2;
    }
    else
    {
        maxIterIls = n;
    }

    cout << "Dimension: " << n << endl;
    cout << "DistanceMatrix: " << endl;
    data.printMatrixDist();

    //cout << "Exemplo de Solucao s = ";
      
    best =  Construcao(data, n);
    // calcularValorObj(&best, data);
    //cout << n << " -> " << 1 << endl;
    exibirSolucao(&best);
    calcularValorObj(best, data);
    

    return 0;
}