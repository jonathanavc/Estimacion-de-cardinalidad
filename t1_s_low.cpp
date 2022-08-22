//g++ t1.cpp
#include <bits/stdc++.h>

using namespace std;

double correcion = 0.7;
size_t max_1 = (SIZE_MAX>>63)<<63;

struct sketch{
    unsigned int sketch: 7;    //  7 bits
};

sketch zeros(size_t s_hashed, sketch k){
    sketch cont = {k};
    while (((s_hashed <<cont.sketch) & max_1) != max_1 && cont.sketch != 64 - k.sketch) cont.sketch++;
    cont.sketch -= (k.sketch - 1);
    return cont;
}

void update(sketch * sketches, size_t s_k, size_t s_hashed, sketch k){
    sketch n_zeros = zeros(s_hashed, k);
    if(n_zeros.sketch > sketches[s_k].sketch) sketches[s_k].sketch = n_zeros.sketch;
}

int main(int argc, char const *argv[]){
    if(argc != 3){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_clusters\"" << endl;
    }
    sketch k = {(unsigned int)atoi(argv[2])};
    sketch * sketches = new sketch[(int)pow(2,k.sketch)];
    for (size_t i = 0; i < pow(2,k.sketch); i++) sketches[i].sketch = 0;
    string s;
    fstream in;
    
    in.open(argv[1]);
    while (in >> s){
        size_t s_hashed = hash<string>{}(s);
        size_t s_k = s_hashed >> (64 - k.sketch);
        if(k.sketch == 0) s_k = 0;
        update(sketches, s_k, s_hashed, k);
    }
    sketch sum = {0};
    for (size_t i = 0; i < pow(2,k.sketch); i++){
        cout <<"buck_"<<i+1<<": "<< sketches[i].sketch << endl;
        sum.sketch += sketches[i].sketch;
    }
    cout << "res: " << pow(2, sum.sketch/pow(2, k.sketch)) * correcion << endl;
    return 0;
}