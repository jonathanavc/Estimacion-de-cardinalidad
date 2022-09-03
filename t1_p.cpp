//g++ t1_p.cpp -std=c++11 -lpthread -O3
#include <bits/stdc++.h>

using namespace std;

mutex _mutex;
double correcion = 0.7; //////////////////////arreglar
unsigned short k_mers = 31;
size_t max_1 = (SIZE_MAX>>63)<<63;
size_t _cont = 0;
chrono::_V2::system_clock::time_point start;


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
    in.open(f_name, ios::in);
    in.seekg(0, ios::end);

    size_t size = in.tellg();
    size_t max = size / n_threads;
    size_t beg = id * max;
    size_t cont = 0;
    size_t cont_2 = 0;
    size_t lines = 0;

    in.seekg(beg, ios::beg);
    string aux;
    while (in >> aux && cont < max){
        cont += aux.length();
        // if(cont > max); ///////////////////////////quitar chars
        // if(lines%10000 == 0 && lines != 0){
        //     chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
        //     _mutex.lock();
        //     _cont += cont_2;
        //     system("clear");
        //     cout <<"["<< ((float)_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)_cont/size) - duration.count()/60000 <<"m"<< endl;
        //     _mutex.unlock();
        //     cont_2 = 0;
        // }
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
        lines++;
        cont_2 += aux.length();
    }
    /*
    while (in >> c && cont < max){
        if(cont%(size / 1000) == 0 && cont != 0){
            system("clear");
            chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
            _mutex.lock();
            _cont += (size / 1000);
            cout <<"["<< ((float)_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)_cont/size) - duration.count()/60000 <<"m"<< endl;
            _mutex.unlock();
        }
        if(c != 'A' && c != 'C' && c != 'T' && c != 'G') {
            s_queue = queue<char>();            //vaciar cola;
        }
        else{
            if(s_queue.size() == k_mers) s_queue.pop();
            s_queue.push(c);
            if(s_queue.size() == k_mers){
                string s;
                for (short i = 0; i < k_mers; i++){
                    char aux = s_queue.front();
                    s_queue.pop();
                    s.push_back(aux);
                    s_queue.push(aux);
                }
                //cout << s << endl;
                size_t s_hashed = hash<string>{}(s);
                size_t s_k = s_hashed >> (64 - k);
                if(k == 0) s_k = 0;
                update(b, s_k, s_hashed, k);
            }
        }
        cont++;
        */
}
//////////////////////arreglar

int main(int argc, char const *argv[]){
    if(argc != 4){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_buckets < 64\" \"N°threads < 256\"" << endl;
        return 1;
    }
    unsigned short k = atoi(argv[2]);
    if(k > 64 || k < 0){
        cout <<  "N°bits_buckets < 64" << endl;
        return 1;
    }
    unsigned short n_threads = atoi(argv[3]);
    unsigned short k_pow = 1<<k;
    unsigned short * b = new unsigned short[k_pow];
    thread threads[n_threads];
    for (size_t i = 0; i < k_pow; i++) b[i] = 0;
    size_t sum = 0;

    start = chrono::system_clock::now();
    for (size_t i = 0; i < n_threads; i++) 
        threads[i] = thread(read, i, (string)argv[1], b, k, n_threads);
    read(n_threads, (string)argv[1], b, k, n_threads);
    // sincronizacion
    for (size_t i = 0; i < n_threads; i++) 
        if(threads[i].joinable()) 
            threads[i].join();

    //////////////////////arreglar
    for (size_t i = 0; i < k_pow; i++){
        cout <<"buck_"<<i+1<<": "<< b[i] << endl;
        sum += b[i];
    }
    cout << "res: " << pow(2, (int)(sum / k_pow)) * correcion << endl;
    //////////////////////arreglar
    chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
    cout <<"["<< "%] Tiempo restante "<< duration.count()/60000 <<"m"<< endl;
    return 0;
}