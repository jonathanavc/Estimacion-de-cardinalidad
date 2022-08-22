//g++ t1.cpp
#include <bits/stdc++.h>

using namespace std;

double correcion = 0.7;
size_t max_1 = (SIZE_MAX>>63)<<63;

unsigned short zeros(size_t s_hashed, size_t k){
    unsigned short cont = k;
    while (((s_hashed <<cont) & max_1) != max_1 && cont != 64 - k) cont++;
    return cont - k + 1;
}

void update(unsigned short * b, size_t s_k, size_t s_hashed, size_t k){
    unsigned short n_zeros = zeros(s_hashed, k);
    if(n_zeros > b[s_k]) b[s_k] = n_zeros;
}

int main(int argc, char const *argv[]){
    if(argc != 3){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_clusters\"" << endl;
    }
    size_t k = atoi(argv[2]);
    unsigned short * b = new unsigned short[(int)pow(2,k)];
    for (size_t i = 0; i < (int)pow(2,k); i++) b[i] = 0;
    string s;
    fstream in;
    
    in.open(argv[1]);
    while (in >> s){
        size_t s_hashed = hash<string>{}(s);
        size_t s_k = s_hashed >> (64 - k);
        if(k == 0) s_k = 0;
        update(b, s_k, s_hashed, k);
    }
    size_t sum = 0;
    for (int i = 0; i < (int)pow(2,k); i++){
        cout <<"buck_"<<i+1<<": "<< b[i] << endl;
        sum += b[i];
    }
    cout << "res: " << pow(2, sum/k) * correcion << endl;
    return 0;
}