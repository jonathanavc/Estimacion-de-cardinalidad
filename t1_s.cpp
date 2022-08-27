//g++ t1.cpp
#include <bits/stdc++.h>

using namespace std;

double correcion = 0.7; //////////////////////arreglar
unsigned short k_mers = 31;
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
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_buckets < 64\"" << endl;
        return 1;
    }
    int sum = 0;
    unsigned short k = atoi(argv[2]);
    if(k > 64 || k < 0){
        cout <<  "N°bits_buckets < 64" << endl;
        return 1;
    }
    size_t k_pow = 1<<k;
    unsigned short * b = new unsigned short[k_pow];
    for (size_t i = 0; i < k_pow; i++) b[i] = 0;

    fstream in(argv[1], ios::in);

    in.seekg(0, ios::end);

    size_t size = in.tellg();
    size_t cont = 0;
    size_t lines = 0;
    chrono::_V2::system_clock::time_point start = chrono::system_clock::now();

    in.seekg(0, ios::beg);

    if(in.is_open()){
        string aux;
        while (in >> aux){
            if(lines%10000 == 0 && lines != 0){
                system("clear");
                chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
                cout <<"["<< ((float)cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)cont/size) - duration.count()/60000 <<"m"<< endl;
            }
            if(aux.length() >= k_mers){
                for(short i = 0; i < aux.length() - k_mers + 1; i++){
                    string s;
                    bool valido = 1;
                    for (short j = i; j < i + k_mers; j++){
                        if(aux[j] != 'A' && aux[j] != 'C' && aux[j] != 'T' && aux[j] != 'G'){
                            valido = false;
                            break;
                        }
                        s.push_back(aux[j]);
                    }
                    if(valido){
                        size_t s_hashed = hash<string>{}(s);
                        size_t s_k = s_hashed >> (64 - k);
                        if(k == 0) s_k = 0;
                        update(b, s_k, s_hashed, k);
                    }
                }
            }
            cont += aux.length();
            lines++;
        }
        ///////////////////////////arreglar
        for (size_t i = 0; i < k_pow; i++){ 
            cout <<"buck_"<<i+1<<": "<< b[i] << endl;
            sum += b[i];
        }
        cout << "res: " << pow(2, (int)(sum / k_pow)) * correcion << endl;
        ///////////////////////////arreglar
    }
    else{
        cout << "No se encontró el archivo " << argv[1] << endl;
        return 1;
    }
    return 0;
}