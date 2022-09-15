#include <bits/stdc++.h>

class cardinalidad{
    protected:
        bool resultado_ready;
        bool calcular_ready;
        size_t k_pow;
        size_t global_cont;
        unsigned short k;
        unsigned short k_mers;
        unsigned short _thread;
        unsigned short n_threads;
        std::string f_name;
        std::mutex _mutex;
        std::chrono::_V2::system_clock::time_point start;
        virtual void update(int id, size_t s_k, size_t s_hashed) = 0;
        void read(int id){
            std::fstream archivo_entrada(f_name, std::ios_base::in);
            archivo_entrada.seekg(0, std::ios::end);
            size_t size = archivo_entrada.tellg();
            size_t max = archivo_entrada.tellg() / n_threads;
            size_t beg = id * max;
            if(id == n_threads - 1) max = SIZE_MAX;
            size_t thread_cont = 0;
            size_t aux_cont = 0;
            size_t lines = 0;
            archivo_entrada.seekg(beg, std::ios::beg);
            std::string aux;
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
                        std::cout << "\33[2K\r";
                        std::chrono::duration<float,std::milli> duration = std::chrono::system_clock::now() - start;
                        std::cout <<"["<< ((float)global_cont/size)*100 << "%] Tiempo restante "<< (duration.count()/60000)/((float)global_cont/size) - duration.count()/60000 <<"m"<< std::endl;
                        std::cout<<"\e[A";
                        _thread = 0;
                    }
                    aux_cont = 0;
                }
                //PROCESAR SI EL TEXTO ES MAYOR A K_MERS
                if(aux.length() >= k_mers){
                    //SE DIVIDE EL TEXTO EN SUB-STRING DE TAMAÑO K_MERS
                    for(short i = 0; i < aux.length() - k_mers + 1; i++){
                        std::string s;
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
                            size_t s_hashed = std::hash<std::string>{}(s);
                            size_t s_k = s_hashed >> (64 - k);
                            if(k == 0) s_k = 0;
                            update(id, s_k, s_hashed);
                        }
                    }
                }
                thread_cont += aux.length();
                aux_cont += aux.length();
                lines++;
            }
        }
    public:
        explicit cardinalidad(std::string f_name, unsigned short k, unsigned short n_threads, unsigned short k_mers = 31){
            this->f_name = f_name;
            this->k = k;
            this->k_mers = k_mers;
            this->k_pow = (size_t)1<<k;
            this->n_threads = n_threads;

            _thread = 0;
            global_cont = 0;
            calcular_ready = 0;
            resultado_ready = 0;
        }
        ~cardinalidad(){
        }
        int calcular(){
            resultado_ready = 0;
            calcular_ready = 0;
            global_cont = 0;
            _thread = 0;
            std::fstream archivo_entrada(f_name, std::ios_base::in);

            if(!archivo_entrada.is_open()) return 0;

            std::thread threads[n_threads];
            start = std::chrono::system_clock::now();
            for (size_t i = 0; i < n_threads; i++) threads[i] = std::thread(&cardinalidad::read, this, i);
            for (size_t i = 0; i < n_threads; i++) if(threads[i].joinable()) threads[i].join();
            auto duration = std::chrono::system_clock::now() - start;
            std::cout << "\33[2K\r";
            std::cout <<"[100%]"<< "Tiempo total:" << std::chrono::duration_cast<std::chrono::seconds>(duration).count()/60 <<"m "
                << std::chrono::duration_cast<std::chrono::seconds>(duration).count()%60 <<"s" << std::endl;
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()/(double)1000 <<"s;"; 
            calcular_ready = 1;
            return 1;
        }
};

class hyperloglog: public cardinalidad{
    private:
        double alpha;
        unsigned short * buckets = new unsigned short[k_pow];
        unsigned short * b = new unsigned short[k_pow * n_threads];
    public:
        hyperloglog(std::string f_name, unsigned short k, unsigned short n_threads, unsigned short k_mers = 31):cardinalidad(f_name,  k, n_threads, k_mers){
            if(k_pow == 16) alpha = 0.673;
            else if(k_pow == 32) alpha = 0.697;
            else if(k_pow == 64) alpha = 0.709;
            else alpha = 0.7213 / (1.0 + 1.079 / k_pow);

            buckets = new unsigned short[k_pow];
            b = new unsigned short[k_pow * n_threads];
            for(size_t i = 0; i < k_pow; i++) buckets[i] = 0;
            for(size_t i = 0; i < k_pow * n_threads; i++) b[i] = 0;
        }
        ~hyperloglog(){
            delete b;
            delete buckets;
        }
        void update(int id, size_t s_k, size_t s_hashed) override{
            unsigned short n_zeros = __builtin_clzll(s_hashed<<k) + 1;
            if(n_zeros > b[id * k_pow + s_k]) b[id * k_pow + s_k] = n_zeros;
        }
        double resultado(){
            if(!calcular_ready) if(!calcular()) return 0;
            for (size_t i = 0; i < k_pow; i++){
                unsigned short _max = 0;
                for (size_t j = 0; j < n_threads; j++) _max = std::max(b[j * k_pow + i], _max);
                buckets[i] = _max;
            }
            size_t z_buckets = 0;
            double res = 0;
            for (size_t i = 0; i < k_pow; i++){
                if(z_buckets == 0) z_buckets++;
                res += pow(2,-buckets[i]);
            }
            res = (pow(k_pow,2)/res) * alpha;
            resultado_ready = 1;
            if (res <= 5/2 * k_pow && z_buckets != 0) return k_pow*log(k_pow/(double)z_buckets);
            else if (res <= 1/30 * pow(2,64)) return res;
            else return -pow(2,64) * log(1.0 - (res/pow(2,64)));
        }
        unsigned short * giveme_buck(){
            if(!resultado_ready){
                if(resultado()) return buckets;
                return NULL;
            }
            return buckets;
        }
};

class pcsa: public cardinalidad{
    private:
        size_t * buckets;
        size_t * b;
        size_t R(size_t x){ //ahora si ta bien
	        return ~x & (x+1);
        }
    public:
        pcsa(std::string f_name, unsigned short k, unsigned short n_threads, unsigned short k_mers = 31):cardinalidad(f_name,  k, n_threads, k_mers){
            buckets = new size_t[k_pow];
            b = new size_t[k_pow * n_threads];
            for(size_t i = 0; i < k_pow; i++) buckets[i] = 0;
            for(size_t i = 0; i < k_pow * n_threads; i++) b[i] = 0;
        }
        ~pcsa(){
            delete b;
            delete buckets;
        }
        using cardinalidad::cardinalidad;
        void update(int id, size_t s_k, size_t s_hashed) override{
            b[id * k_pow + s_k] = b[id * k_pow + s_k] | R(s_hashed);
        }
        double resultado(){
            double phi = 0.77351;
            if(!calcular_ready) if(!calcular()) return 0;
            for (size_t i = 0; i < k_pow; i++){
                size_t i_sketch = 0;
                for (size_t j = 0; j < n_threads; j++) i_sketch = i_sketch | b[j * k_pow + i];
                buckets[i] = i_sketch;
            }
            double res = 0;
            for (size_t i = 0; i < k_pow; i++) res += __builtin_ctzll(~buckets[i]);
            res = res/k_pow;
            resultado_ready = 1;
            return (k_pow*pow(2,res)/phi);
        }
};