//g++ t1_p.cpp -std=c++11 -lpthread -O3
#include <bits/stdc++.h>

using namespace std;

mutex _mutex;
double correcion = 0.7; //////////////////////arreglar
unsigned short k_mers = 31;
size_t _cont = 0;
short _thread = 0;
chrono::_V2::system_clock::time_point start;



void update(int id, unsigned short * b, size_t s_k, size_t s_hashed, size_t k_pow, unsigned short k){
    unsigned short n_zeros = __builtin_clzll(s_hashed<<k);
    if(n_zeros > b[id * k_pow + s_k]) b[id * k_pow + s_k] = n_zeros;
}

void read(int id, string f_name , unsigned short * b, unsigned short k, unsigned short n_threads){
    fstream in;
    in.open(f_name, ios::in);
    in.seekg(0, ios::end);

    size_t size = in.tellg();
    size_t max = in.tellg() / n_threads;
    size_t beg = id * max;
    if(id == n_threads - 1) max = SIZE_MAX;
    size_t cont = 0;
    size_t cont_aux = 0;
    size_t lines = 0;

    in.seekg(beg, ios::beg);
    string aux;
    
    while (in >> aux && cont < max){
        //TODO EL TEXTO LEIDO A MAYUSCULAS
        transform(aux.begin(),aux.end(),aux.begin(),::toupper);
        cont += aux.length();

        //TEXTO DE PROGRESO
        if(lines%10000 == 0 && lines != 0){
            bool _cout = 0;

            _mutex.lock();

            _cont += cont_aux;
            _thread++;
            if(_thread == n_threads){
                _cout = 1;
                _thread = 0;
            }

            _mutex.unlock();

            if(_cout){
                chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
                system("clear");
                cout <<"["<< ((float)_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)_cont/size) - duration.count()/60000 <<"m"<< endl;
                _thread = 0;
            }
            cont_aux = 0;
        }

        //PROCESAR SI EL TEXTO ES MAYOR A K_MERS
        if(aux.length() >= k_mers){
            //SE DIVIDE EL TEXTO EN SUB-STRING DE TAMAÑO K_MERS
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
                //SI EL TEXTO SOLO CONTIENE CARACTERES VALIDOS SE PROCESA
                if(valido){
                    size_t s_hashed = hash<string>{}(s);
                    size_t s_k = s_hashed >> (64 - k);
                    if(k == 0) s_k = 0;
                    update(id, b, s_k, s_hashed, (size_t)1<<k, k);
                }
            }
        }
        lines++;
        cont_aux += aux.length();
    }
}

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
    size_t k_pow = (size_t)1<<k;                                        // = 2^K
    unsigned short * b = new unsigned short[k_pow * n_threads];         // BUCKETS
    thread threads[n_threads];
    for (size_t i = 0; i < k_pow; i++) b[i] = 0;

    start = chrono::system_clock::now();
    for (size_t i = 0; i < n_threads; i++) threads[i] = thread(read, i, (string)argv[1], b, k, n_threads);
    for (size_t i = 0; i < n_threads; i++) if(threads[i].joinable()) threads[i].join();
    auto duration = chrono::system_clock::now() - start;

    cout <<"[100%]"<< "Tiempo total:" << std::chrono::duration_cast<std::chrono::seconds>(duration).count()/60 <<"m "
        << std::chrono::duration_cast<std::chrono::seconds>(duration).count()%60 <<"s" << endl;

    //////////////////////ojo
    unsigned short buckets[k_pow];
    for (size_t i = 0; i < k_pow; i++){
        unsigned short _max = 0;
        for (size_t j = 0; j < n_threads; j++){
            _max = max(b[j * k_pow + i], _max);
        }
        buckets[i] = _max;
    }
    
    double sum = 0;
    for (size_t i = 0; i < k_pow; i++){
        cout <<"buck_"<<i+1<<": "<< buckets[i] << endl;
        sum += (double)1/(double)buckets[i];
        cout << "sum:" << sum << endl;
    }

    
    double res = pow(2, (double)k_pow/sum) * correcion;
    cout << "res: " << res << endl;
    //////////////////////ojo

    free(b);
    return 0;
}