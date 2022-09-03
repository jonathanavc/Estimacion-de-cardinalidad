#include <bits/stdc++.h>

using namespace std;

bool isCommentLine(string line) {
    return (line[0] == '>');        
}

int linesPerThread(unsigned long fileSize, unsigned int lineWidth, unsigned int threadCount) {
    int aproxLineCount = fileSize / lineWidth;
    return aproxLineCount / threadCount; 
}

mutex _mutex;
double correcion = 0.7; //////////////////////arreglar
unsigned short k_mers = 31;
size_t max_1 = (SIZE_MAX>>63)<<63;
size_t _cont = 0;
chrono::_V2::system_clock::time_point start;
char lowerCaseNucleotides[4] = { 'a', 't', 'c', 'g' };
char nucleotides[4] = { 'A', 'T', 'C', 'G' };
int k_mer_length = 31;

bool isValidNucleotide(char input) 
{
    for (int i = 0; i < 4; i++)
        if (input == nucleotides[i])
            return true;
    return false;        
}

unsigned short zeros(size_t s_hashed, size_t k){
    unsigned short cont = k;
    while (((s_hashed <<cont) & max_1) != max_1 && cont != 64 - k) cont++;
    return cont - k + 1;
}

void update(unsigned short * b, size_t s_k, size_t s_hashed, size_t k){
    unsigned short n_zeros = zeros(s_hashed, k);
    _mutex.lock();
    if(n_zeros > b[s_k]) 
        b[s_k] = n_zeros;
    _mutex.unlock();
}

struct kvpBucketZeros {
    size_t s_k;
    size_t n_zeros;
};

void batchUpdate(unsigned short * b, size_t s_k, size_t s_hashed, size_t k, std::vector<kvpBucketZeros>* batches){
    unsigned short n_zeros = zeros(s_hashed, k);
    kvpBucketZeros kvp;
    kvp.s_k = s_k;
    kvp.n_zeros = zeros(s_hashed, k);
    batches->push_back(kvp);
    if (batches->size() > 100000) {
        _mutex.lock();
        for (size_t i = 0; i < 100001; i++)
        {    
            kvpBucketZeros currKvp = batches->front();
            batches->pop_back();        
            if(currKvp.n_zeros > b[currKvp.s_k]) 
                b[currKvp.s_k] = currKvp.n_zeros;            
        }  
        _mutex.unlock();      
    }
}

void SkipLines(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
}

void processLine(string& line, unsigned short *b, unsigned short k, std::vector<kvpBucketZeros>* batches){
    string kmerBuffer;
    int kmer_start = 0;
    int kmer_current_count = 0; 
    for (std::string::iterator it = line.begin(); it != line.end(); ++it)
    {
        char upperCaseNucleotide = toupper(*it);
        if (!isValidNucleotide(upperCaseNucleotide))
            continue;
        kmerBuffer.push_back(upperCaseNucleotide);
        kmer_current_count++;
        if (kmer_current_count == 31)
        {
            size_t s_hashed = hash<string>{}(&kmerBuffer[kmer_start]);
            size_t s_k = s_hashed >> (64 - k);
            if(k == 0) 
                s_k = 0;
            batchUpdate(b, s_k, s_hashed, k, batches); 
            kmer_start++;
            kmer_current_count--;
        }        
    }
}

void read(int thread_id, string f_name, int linesPerThread, unsigned short * b, unsigned short k, unsigned short n_threads){
    string s;
    fstream file;
    // int firstLine = thread_id * linesPerThread;
    int lastLine = (thread_id + 1) * linesPerThread - 1;
    int linesToBeSkipped = linesPerThread * thread_id;
    bool readToEnd = thread_id == n_threads;
    string buffer;
    file.open(f_name, ios::in);
    
    size_t currentLine = linesToBeSkipped;
    SkipLines(file, linesToBeSkipped);
    std::vector<kvpBucketZeros>* batches =  new std::vector<kvpBucketZeros>();
            
    while (file >> buffer && (currentLine < lastLine || readToEnd)){
        currentLine++;
        if (buffer[0] == '>')
            continue;
        processLine(buffer, b, k, batches);
    }
}

int main(int argc, char const *argv[]){
    if(argc != 4){
        std::cout << "Modo de uso " << argv[0] << "  \"nombre_archivo\"  \"(int)N°bits_buckets < 64\" \"N°threads < 256\"" << std::endl;
        return 1;
    }
    unsigned short k = atoi(argv[2]);
    if(k > 64 || k < 0){
        std::cout <<  "N°bits_buckets < 64" << std::endl;
        return 1;
    }
    unsigned short n_threads = atoi(argv[3]);
    unsigned short k_pow = ((size_t)1)<<k;
    unsigned short * b = new unsigned short[k_pow];
    thread threads[n_threads];
    for (size_t i = 0; i < k_pow; i++) 
        b[i] = 0;
    size_t sum = 0;

    
    // preprocesar archivo
    string f_name = (string)argv[1];
    fstream in;
    in.open(f_name, ios::in);
    in.seekg(0, ios::end);

    size_t totalCharacterCount = in.tellg();
    in.seekg(0, ios::beg);
    string line;
    int lineWidth = 81; 

    // lines per thread
    std::cout << "Ancho de linea: " << lineWidth << std::endl;
    std::cout << "Total de carácteres: " << totalCharacterCount << std::endl;
    int linesThread = linesPerThread(totalCharacterCount, lineWidth, n_threads);
    std::cout << "Threads: " << n_threads << std::endl;
    std::cout << "Lineas por thread: " << linesThread << std::endl;
    start = chrono::system_clock::now();
    for (size_t i = 0; i < n_threads; i++) 
        threads[i] = thread(read, i, (string)argv[1], linesThread, b, k, n_threads);    
    // sincronizacion
    for (size_t i = 0; i < n_threads; i++) 
        if(threads[i].joinable()) 
            threads[i].join();

    //////////////////////arreglar
    for (size_t i = 0; i < k_pow; i++){
        std::cout <<"buck_"<<i+1<<": "<< b[i] << std::endl;
        sum += b[i];
    }
    std::cout << "res: " << pow(2, (int)(sum / k_pow)) * correcion << std::endl;
    //////////////////////arreglar
    chrono::duration<float,milli> duration = chrono::system_clock::now() - start;
    std::cout <<"["<< "%] Tiempo restante "<< duration.count()/60000 <<"m"<< std::endl;
    return 0;
}