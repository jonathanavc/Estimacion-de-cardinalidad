//g++ main.cpp -std=c++11 -lpthread -O3 -g
#include <bits/stdc++.h>
#include "cardinalidad.cpp"

using namespace std;

int main(int argc, char const *argv[]){
    string fn_g1 = "GCF_000001405.39_GRCh38.p13_genomic.fna";
    string fn_g2 = "GCF_000308155.1_EptFus1.0_genomic.fna";

    unsigned short k = 14; //N° de bits para buckets
    unsigned short k_mers = 31;
    unsigned short n_threads = 8;

    hyperloglog g1(fn_g1, k, n_threads, k_mers);
    g1.calcular();
    cout << g1.resultado() << endl;

    hyperloglog g2(fn_g2, k, n_threads, k_mers);
    g2.calcular();
    cout << g2.resultado() << endl;

    cout << _union(k, &g1, &g2) << endl;
    cout << "jaccard: "<< _jaccard(k, &g1, &g2) <<endl;

    return 0;
}