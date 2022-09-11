//g++ hll_p.cpp -std=c++11 -lpthread -O3
#include <bits/stdc++.h>

using namespace std;

mutex _mutex;
unsigned short k_mers = 31;
size_t global_cont = 0;
short _thread = 0;
chrono::_V2::system_clock::time_point start;


void update(int id, unsigned short * b, size_t s_k, size_t s_hashed, size_t k_pow, unsigned short k){
    unsigned short n_zeros = __builtin_clzll(s_hashed<<k) + 1;
    if(n_zeros > b[id * k_pow + s_k]) b[id * k_pow + s_k] = n_zeros;
}

void read(int id, string f_name , unsigned short * b, unsigned short k, unsigned short n_threads){
    fstream archivo_entrada;
    archivo_entrada.open(f_name, ios::in);
    archivo_entrada.seekg(0, ios::end);

    size_t size = archivo_entrada.tellg();
    size_t max = archivo_entrada.tellg() / n_threads;
    size_t beg = id * max;
    if(id == n_threads - 1) max = SIZE_MAX;
    size_t thread_cont = 0;
    size_t aux_cont = 0;
    size_t lines = 0;

    archivo_entrada.seekg(beg, ios::beg);
    string aux;
    
    while (archivo_entrada >> aux && thread_cont < max){
        //TODO EL TEXTO LEIDO A MAYUSCULAS
        transform(aux.begin(),aux.end(),aux.begin(),::toupper);
        //TEXTO DE PROGRESO
        if(lines%10000 == 0 && lines != 0){
            bool _cout = 0;

            _mutex.lock();
            global_cont += aux_cont;
            _thread++;
            if(_thread == n_threads){
                _cout = 1;
                _thread = 0;
            }
            _mutex.unlock();

            if(_cout){
                printf("\33[2K\r");
                chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
                cout <<"["<< ((float)global_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)global_cont/size) - duration.count()/60000 <<"m"<< endl;
                cout<<"\e[A";
                _thread = 0;
            }
            aux_cont = 0;
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
        thread_cont += aux.length();
        aux_cont += aux.length();
        lines++;
    }
}


double resultado(unsigned short * b, size_t k_pow, unsigned short n_threads){
    double alpha;
    unsigned short buckets[k_pow];
    for (size_t i = 0; i < k_pow; i++){
        unsigned short _max = 0;
        for (size_t j = 0; j < n_threads; j++) _max = max(b[j * k_pow + i], _max);
        buckets[i] = _max;
    }
    double res = 0;
    for (size_t i = 0; i < k_pow; i++){
        res += 1/pow(2,buckets[i]);
    }
    switch (k_pow) {
        case 16:
            alpha = 0.673;
            break;
        case 32:
            alpha = 0.697;
            break;
        case 64:
            alpha = 0.709;
            break;
        default:
            alpha = 0.7213 / (1.0 + 1.079 / k_pow);
            break;
    }
    return (pow(k_pow, 2)/res) * alpha;
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
    for (size_t i = 0; i < k_pow * n_threads; i++) b[i] = 0;

    start = chrono::system_clock::now();
    for (size_t i = 0; i < n_threads; i++) threads[i] = thread(read, i, (string)argv[1], b, k, n_threads);
    for (size_t i = 0; i < n_threads; i++) if(threads[i].joinable()) threads[i].join();
    auto duration = chrono::system_clock::now() - start;

    printf("\33[2K\r");
    cout <<"[100%]"<< "Tiempo total:" << std::chrono::duration_cast<std::chrono::seconds>(duration).count()/60 <<"m "
        << std::chrono::duration_cast<std::chrono::seconds>(duration).count()%60 <<"s" << endl;
    cout << "res: " << resultado(b, k_pow, n_threads) << endl;

    free(b);
    return 0;
}