//g++ t1_p.cpp -std=c++11 -lpthread -O3
#include <bits/stdc++.h>

using namespace std;

mutex _mutex;
short length_line = 81;
double correcion = 0.7; //////////////////////arreglar
size_t max_1 = (SIZE_MAX>>63)<<63;

unsigned short zeros(size_t s_hashed, size_t k){
    unsigned short cont = k;
    while (((s_hashed <<cont) & max_1) != max_1 && cont != 64 - k) cont++;
    return cont - k + 1;
}

void update(unsigned short * b, size_t s_k, size_t s_hashed, size_t k){
    unsigned short n_zeros = zeros(s_hashed, k);
    _mutex.lock();
    if(n_zeros > b[s_k]) b[s_k] = n_zeros;
    _mutex.unlock();
}

//////////////////////arreglar (no se lee todo el archivo)

void read(int id, string f_name , unsigned short * b, unsigned short k, unsigned short n_threads){
    string s;
    fstream in;
    in.open(f_name);
    in.seekg(0, ios::end);

    size_t size = in.tellg();
    size_t cont = 0;

    size_t max = size/(length_line * n_threads);
    size_t beg = id * max;

    _mutex.lock();
    cout << "id:" << id << ",beg:" << beg << endl;
    _mutex.unlock();
    //size_t move = (int)length_line * ((int)pow(2, k) - 1);
    in.seekg(beg, ios::beg);
    while (in >> s && cont < max){
        if(s.length()!=80) {
            _mutex.lock();
            cout <<"Error id:"<< id << "s:" << s << endl;
            _mutex.unlock();    
        }
        size_t s_hashed = hash<string>{}(s);
        size_t s_k = s_hashed >> (64 - k);
        if(k == 0) s_k = 0;
        update(b, s_k, s_hashed, k);
        cont++;
    }
}
//////////////////////arreglar

int main(int argc, char const *argv[]){
    if(argc != 4){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_buckets < 64\" \"N°threads < 256\"" << endl;
    }
    unsigned short k = atoi(argv[2]);
    unsigned short n_threads = atoi(argv[3]);
    unsigned short k_pow = 1<<k;
    unsigned short * b = new unsigned short[k_pow];
    thread threads[n_threads];
    for (size_t i = 0; i < k_pow; i++) b[i] = 0;
    size_t sum = 0;

    for (size_t i = 0; i < n_threads; i++) threads[i] = thread(read, i, (string)argv[1], b, k, n_threads);
    read(n_threads, (string)argv[1], b, k, n_threads);
    for (size_t i = 0; i < n_threads; i++) if(threads[i].joinable()) threads[i].join();

//////////////////////arreglar
    for (size_t i = 0; i < k_pow; i++){
        cout <<"buck_"<<i+1<<": "<< b[i] << endl;
        sum += b[i];
    }
    cout << "res: " << pow(2, (int)(sum / k_pow)) * correcion << endl;
//////////////////////arreglar
    return 0;
}