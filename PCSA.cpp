#include <bits/stdc++.h>
using namespace std;

const double phi = 0.77351; //factor de correción
const double error = 0.05;  //error arbitrario para despejar m
const double m = 0.78/error; //despeje
const int M = ceil(m);
long long int *sketch = new long long int[M];
hash<string> h1;

unsigned short k_mers = 31;
int shiffting = 64 - k_mers;
long long int R(long x){ //ta bien
	return ~x & (x+1);
}

void update(string linea, int k_mers){ //este k generalizar a desplazamiento
	long long int x = h1(linea);
	long long int aux = __builtin_ctzll(x); //generalizar
	sketch[aux] = sketch[aux] | R(x);
}	

long long int estimacion(){ //se supone que ta bien
	int sum = 0;
	for(int i = 0; i < M; i++){
		sum += R(sketch[i]);
	}
	double media = (1.0*sum)/M;
	return (M*pow(2,media)/phi);
}

int main(int argc, char const *argv[]){
	if(argc != 2){
        cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"" << endl;
        return 1;
    } //If para verificar la entrada
    for (size_t i = 0; i < M; i++) sketch[i] = 0;

    ifstream file; //crear variable para el archivo
    file.open(argv[1]); //abrir el archivo
    string linea; //string para almacenar las lineas
    // cout << "ahora empezaré :)" << endl;
    double t0 = clock();
    int y = 0;
    while(file >> linea){ //while para leer el archivo
    	if(linea[0] == '>'){ //si la linea empieza con ese char, entonces se la salta
    		continue;
    	}
        // cout << "while: " << y << endl;
    	transform(linea.begin(), linea.end(), linea.begin(), ::toupper); //convierte toda la linea en mayusula
        // cout << linea << endl;

    	if(linea.length() >= k_mers){ //si la linea tiene más caracteres que el k_mers permitido entonces
    		for(short i = 0; i < linea.length() - k_mers + 1; i++){ //for para recorrer la linea con tam maximo de k_mers
                // cout << "for" << i << endl;
                string s; //string aux
                bool valido = 1; //bool aux
                for (short j = i; j < i + k_mers; j++){//for para ver char a char de la linea
                    // cout << "J: " << j << endl;
                    // cout << "ancho linea: " << linea.length() << endl;
                    if(linea[j] != 'A' && linea[j] != 'C' && linea[j] != 'T' && linea[j] != 'G'){ //valida si es una base
                        valido = false; //cambia el valor del bool
                        break; //termina ese char
                    }
                    s.push_back(linea[j]); //agrego el char al string aux si es que es validado
                }
                if(valido){ //si es validado el kmers
                    update(s, k_mers); //llamo a la función update (acá adentro se hacen los hash)
                }
            }
    	}
        y++;
    }
    cout <<"Estimación: "<<estimacion() << endl;
    double t1 = clock();
    double time = ((t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time: " << time << endl;
    free(sketch);
	return 0;
}