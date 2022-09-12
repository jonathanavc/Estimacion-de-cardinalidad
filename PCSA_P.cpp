#include <bits/stdc++.h>
using namespace std;

mutex _mutex;
size_t global_cont = 0;
short _thread = 0;
chrono::_V2::system_clock::time_point start;
const double phi = 0.77351; //factor de correción
const double error = 0.05;  //error arbitrario para despejar m
const double m = 0.78/error; //despeje
const int M = ceil(m);

hash<string> h1;

unsigned short k_mers = 31;
int shiffting = 64 - k_mers;

unsigned short R(long x){ //ta bien
	return ~x & (x+1);
}

void update(string lineas, int id, unsigned short *sketch){
	unsigned short x = h1(lineas);
	unsigned short aux = x >> shiffting; //tal vez cambiar por la función __builtin_clzll
	sketch[aux*id] = sketch[aux*id] | R(x);
}	


void read(int id, string f_name , unsigned short * M, unsigned short n_threads){
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
        // //TEXTO DE PROGRESO
        // if(lines%10000 == 0 && lines != 0){
        //     bool _cout = 0;

        //     _mutex.lock();
        //     global_cont += aux_cont;
        //     _thread++;
        //     if(_thread == n_threads){
        //         _cout = 1;
        //         _thread = 0;
        //     }
        //     _mutex.unlock();

        //     if(_cout){
        //         chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
        //         system("clear");
        //         cout <<"["<< ((float)global_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)global_cont/size) - duration.count()/60000 <<"m"<< endl;
        //         _thread = 0;
        //     }
        //     aux_cont = 0;
        // }

        //PROCESAR SI EL TEXTO ES MAYOR A K_MERS
        
        if(aux[0] == '>'){ //si la lineas empieza con ese char, entonces se la salta
            continue;
        }
        if(aux.length() >= k_mers){ //si la lineas tiene más caracteres que el k_mers permitido entonces
            for(short i = 0; i < aux.length() - k_mers + 1; i++){ //for para recorrer la lineas con tam maximo de k_mers
                string s; //string aux
                bool valido = 1; //bool aux
                for (short j = i; j < i + k_mers; j++){//for para ver char a char de la lineas
                    if(aux[j] != 'A' && aux[j] != 'C' && aux[j] != 'T' && aux[j] != 'G'){ //valida si es una base
                        valido = false; //cambia el valor del bool
                        break; //termina ese char
                    }
                    s.push_back(aux[j]); //agrego el char al string aux si es que es validado
                }
                if(valido){ //si es validado el kmers
                    update(s, k_mers, M); //llamo a la función update (acá adentro se hacen los hash)
                }
            }
        }
    
        thread_cont += aux.length();
        aux_cont += aux.length();
        lines++;
    }
}


long long int estimacion(int n_threads, unsigned short * sketch){ //se supone que ta bien
	int sum = 0;
	for(int i = 0; i < M*n_threads; i++){
		sum += R(sketch[i]);
	}
	double media = (1.0*sum)/(M);
	return (M*pow(2,media)/phi);
}

int main(int argc, char const *argv[]){
	if(argc != 3){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"" << endl;
        return 1;
    } //If para verificar la entrada

    unsigned short n_threads = atoi(argv[2]);
    unsigned short *sketch = new unsigned short[M*n_threads];
    
    for (size_t i = 0; i < M*n_threads; i++) sketch[i] = 0;
    
    thread threads[n_threads];
    
    //double t0 = clock();
    start = chrono::system_clock::now();
    for (size_t i = 0; i < n_threads; i++) threads[i] = thread(read, i, (string)argv[1], sketch, n_threads);
    for (size_t i = 0; i < n_threads; i++) if(threads[i].joinable()) threads[i].join();
    
    
    auto duration = chrono::system_clock::now() - start;
    system("clear");
    cout <<"[100%]"<< "Tiempo total:" << std::chrono::duration_cast<std::chrono::seconds>(duration).count()/60 <<"m "
        << std::chrono::duration_cast<std::chrono::seconds>(duration).count()%60 <<"s" << endl;
    cout <<"Estimación: "<<estimacion(n_threads,sketch) << endl; //modificar estimación por tener más buckets por el paralelo
    //double t1 = clock();
    //double time = ((t1-t0)/CLOCKS_PER_SEC);
    
    //cout << "Execution Time: " << time << endl;
    free(sketch);
	return 0;
} //en teoria main listo