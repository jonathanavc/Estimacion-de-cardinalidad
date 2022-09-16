//g++ main.cpp -std=c++11 -lpthread -O3 -g
#include <bits/stdc++.h>
#include "cardinalidad.cpp"

using namespace std;

int main(int argc, char const *argv[]){
    if(argc != 5){
        cout << "Modo de uso "<< argv[0] << "  n°bits_sketch  n°threads  archivo1 archivo2" <<endl;
        return 1;
    }
    string fn_g1 = argv[4];
    string fn_g2 = argv[3];


    unsigned short k = atoi(argv[1]); //N° de bits para sketch
    unsigned short k_mers = 31;
    unsigned short n_threads = atoi(argv[2]);

    hyperloglog g1_hll(fn_g1, k, n_threads);
    pcsa g1_pcsa(fn_g1, k, n_threads);
    hyperloglog g2_hll(fn_g2, k, n_threads, k_mers);
    pcsa g2_pcsa(fn_g2, k, n_threads);

    g1_hll.calcular();
    cout <<"("<< fn_g1 <<")hyperloglog: "<< g1_hll.resultado() << endl;

    g1_pcsa.calcular();
    cout <<"("<< fn_g1 <<")pcsa: "<< g1_pcsa.resultado() << endl;

    g2_hll.calcular();
    cout <<"("<< fn_g2 <<")hyperloglog: "<< g1_hll.resultado() << endl;

    g2_pcsa.calcular();
    cout <<"("<< fn_g2 <<")pcsa: "<< g2_pcsa.resultado() << endl;

    cout << "jaccard: "<< _jaccard(k, &g1_hll, &g2_hll) <<endl;

    return 0;
}