#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <crypt.h>
#include <omp.h>

using namespace std;

class Senha {
private:
    int maxSize;
    string alfa;
    vector<unsigned int> vetor;

    void avancarN(int n) {
        int overflow, pos = 0;
        vetor[pos] += n;
        while ((overflow = (vetor[pos] / maxSize)) > 0 && pos < 8) {
            vetor[pos] %= maxSize;
            if (vetor[pos] == 0)
                vetor[pos] += 1;
            vetor[++pos] += overflow;
        }
    }

public:
    Senha(int comeco) {
        maxSize = 66;
        alfa = "# ./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        vetor = vector<unsigned int>(8, 0);
        avancarN(comeco);
    }

    string getSenha() {
        ostringstream oss;
        for (int i = 0; i < (int)vetor.size(); i++) {
            if (vetor[i] == 0) {
                oss << '\0'; break;
            }
            oss << alfa[vetor[i]];
        }
        return oss.str();
    }

    void prox() {
        avancarN(1);
    }

    void prox(int n) {
        this->avancarN(n);
    }
};

int main(int argc, char *argv[]) {
    // Ler senhas
    char cifra[32] = "Ha0Vy6cN2SuiM";
    vector<string> cifras(400);
    for (int i = 0; i < 400; i++) {
        getline(cin, cifras[i]);
    }

    char *result = new char[32];
    Senha senha(1);
    unsigned long long maximo = 66L * 66L * 66L * 66L * 66L * 66L * 66L * 66L;
    unsigned long long i = 0L;
    for (; i < maximo; i++) {
        // printf("Senha %6d: %s\n", i, senha.getSenha().data());
        for (int j = 0; j < (int)cifras.size(); j++) {
            result = crypt(senha.getSenha().data(), cifras[j].data());
            int ok = strncmp(result, cifras[j].data(), 14) == 0;

            if (ok) {
                printf("(%d) %s == %s\n", 400 - (int)cifras.size(), cifras[j].data(), senha.getSenha().data());
                cifras.erase(cifras.begin() + j);
                j--;
                break;
            }
        }
        if ((int)cifras.size() == 0) break;
        senha.prox();
        if ((i % 100000) == 0) printf("%llu\n", i);
    }
    printf("Quebrou em %llu iterações!!!!\n", i);

    return 0;
}