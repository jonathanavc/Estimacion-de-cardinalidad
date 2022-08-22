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

void update(sketch * b, size_t s_k, size_t s_hashed, sketch k){
    sketch n_zeros = zeros(s_hashed, k);
    if(n_zeros.sketch > b[s_k].sketch) b[s_k].sketch = n_zeros.sketch;
}

int main(int argc, char const *argv[]){
    if(argc != 3){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_clusters\"" << endl;
    }
    sketch k = {(unsigned int)atoi(argv[2])};
    sketch * b = new sketch[(int)pow(2,k.sketch)];
    for (size_t i = 0; i < (int)pow(2,k.sketch); i++) b[i].sketch = 0;
    string s;
    fstream in;
    
    in.open(argv[1]);
    while (in >> s){
        size_t s_hashed = hash<string>{}(s);
        size_t s_k = s_hashed >> (64 - k.sketch);
        if(k.sketch == 0) s_k = 0;
        update(b, s_k, s_hashed, k);
    }
    sketch sum = {0};
    for (int i = 0; i < (int)pow(2,k.sketch); i++){
        cout <<"buck_"<<i+1<<": "<< b[i].sketch << endl;
        sum.sketch += b[i].sketch;
    }
    cout << "res: " << pow(2, sum.sketch/k.sketch) * correcion << endl;
    return 0;
}