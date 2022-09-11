//g++ main.cpp -std=c++11 -lpthread -O3 -g
#include <bits/stdc++.h>
#include "hyperloglog.cpp"

using namespace std;

const string fn_g1 = "GCF_000001405.39_GRCh38.p13_genomic.fna";
const string fn_g2 = "GCF_000308155.1_EptFus1.0_genomic.fna";

double _union(unsigned short k, hyperloglog * g1, hyperloglog * g2){
    size_t k_pow = (size_t)1<<k;
    unsigned short * b_g1 = g1->giveme_buck();
    unsigned short * b_g2 = g2->giveme_buck();
    unsigned short buckets[k_pow];
    double alpha;
    for (size_t i = 0; i < k_pow; i++) buckets[i] = max(b_g1[i], b_g2[i]);
    double res = 0;
    for (size_t i = 0; i < k_pow; i++) res += pow(2,-buckets[i]);
    if(k_pow == 16) alpha = 0.673;
    else if(k_pow == 32) alpha = 0.697;
    else if(k_pow == 64) alpha = 0.709;
    else alpha = 0.7213 / (1.0 + 1.079 / k_pow);
    return (pow(k_pow,2)/res) * alpha;
}

double _jaccard(unsigned short k, hyperloglog * g1, hyperloglog * g2){
    return ( g1->resultado() + g2->resultado() - _union(k, g1, g2) ) / _union(k, g1, g2);
}

int main(int argc, char const *argv[]){
    if(argc != 3){
        cout << "Modo de uso " << argv[0] << "\"(int)N°bits_buckets < 64\" \"N°threads < 256\"" << endl;
        return 1;
    }
    unsigned short k = atoi(argv[1]);
    if(k > 64 || k < 0){
        cout <<  "N°bits_buckets < 64" << endl;
        return 1;
    }
    unsigned short n_threads = atoi(argv[2]);

    hyperloglog g1(fn_g1, k, n_threads);
    g1.calcular();
    cout << g1.resultado() << endl;

    hyperloglog g2(fn_g2, k, n_threads);
    g2.calcular();
    cout << g2.resultado() << endl;

    cout << _union(k, &g1, &g2) << endl;
    cout << "jaccard: "<< _jaccard(k, &g1, &g2) <<endl;
    return 0;
}
