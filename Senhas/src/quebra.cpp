#include "utils.h"
#include "wordgen.h"
#include <algorithm>
#include <crypt.h>
#include <iostream>
#include <map>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <set>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

int stop = 0;
int num_cifras = 0;
std::set<int> falta = std::set<int>();

void mpi_sync(int mpi_rank, int mpi_size, MPI_Comm *comm) {
  // Seção de sincronização de progresso
  int *my_list = new int[num_cifras];
  int *other_list = new int[num_cifras];
  std::set<int> my_set;

  fprintf(stderr, "P%d iniciando sincronia MPI\n", mpi_rank);

  while (falta.size() > 0 && !stop) {
    // Preparar lista de cifras restantes
    memset(my_list, -1, sizeof(int) * num_cifras);
    memset(other_list, -1, sizeof(int) * num_cifras);
#pragma omp critical(falta_global)
    { my_set = falta; }
    int cc = 0;
    for (auto e : my_set) {
      my_list[cc++] = e;
    }

    std::vector<int> new_list;
    sleep_for(2000);
    for (int i = 0; i < mpi_size; i++) {
      if (i == mpi_rank) {
        // Enviar minha lista
        fprintf(stderr, "Processo %d enviando lista\n", mpi_rank);
        MPI_Bcast(my_list, num_cifras, MPI_INT, mpi_rank, *comm);
      } else {
        // Receber lista de alguém
        fprintf(stderr, "Processo %d recebendo lista do processo %d\n",
                mpi_rank, i);
        MPI_Bcast(other_list, num_cifras, MPI_INT, i, *comm);
        std::set<int> other_set;
        for (int j = 0; j < num_cifras; j++) {
          if (other_list[j] > -1)
            other_set.insert(other_list[j]);
        }
        std::set_intersection(my_set.begin(), my_set.end(), other_set.begin(),
                              other_set.end(), std::back_inserter(new_list));
        my_set = std::set<int>(new_list.begin(), new_list.end());
      }
    }
    printf("[%d] removed: ", mpi_rank);
    for (int i = 0; i < num_cifras; i++) {
      if (my_set.count(i) == 0)
        printf("%d ", i);
    }
    printf("\n");
#pragma omp critical(falta_global)
    { falta = my_set; }
  }
}

void force_stop(int signal) {
  fprintf(stderr, "Encerramento forçado: sinal %d\n", signal);
  stop = 1;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, force_stop);

  int thread_level;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &thread_level);

  int mpi_rank = 0;
  int mpi_size = 1;
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &mpi_rank);
  MPI_Comm_size(comm, &mpi_size);

  // Obter comprimento máximo
  int comprimento = 0;
  unsigned long long maximo = 64L;
  if (argc == 2) {
    sscanf(argv[1], "%d", &comprimento);
    comprimento = MIN(8, comprimento);
    for (int i = 1; i < comprimento; i++) {
      maximo++;
      maximo *= (unsigned long long)maxSize;
    }
  } else {
    fprintf(stderr, "Falta argumento: %s <comprimento_maximo> <num_threads>\n",
            argv[0]);
    fprintf(stderr, "Uso: Informe pela entrada padrão o número de cifras, "
                    "número de threads  e em "
                    "seguida digite\n");
    fprintf(stderr, "     as cifras uma por linha.\n");
    exit(1);
  }

  // Ler senhas e sincronizar com outros processos MPI
  int num_cifras = 0;
  std::vector<std::string> sais;
  char **cifras;
  char *cbloco;
  if (mpi_rank == 0) {
    // ROOT
    std::cin >> num_cifras;
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    getchar();
    cifras = new char *[num_cifras];
    cbloco = new char[num_cifras * 32];
    std::string cifra;
    sais = std::vector<std::string>(num_cifras);
    for (int i = 0; i < num_cifras; i++) {
      getline(std::cin, cifra);
      cifras[i] = &cbloco[i * 32];
      strncpy(cifras[i], cifra.data(), 16);
      falta.insert(i);
      sais[i] = cifra.substr(0, 2);
    }
    MPI_Bcast(cifras[0], num_cifras * 32, MPI_CHAR, 0, comm);
  } else {
    // Not root
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    cifras = new char *[num_cifras];
    cbloco = new char[num_cifras * 32];
    sais = std::vector<std::string>(num_cifras);
    MPI_Bcast(cbloco, num_cifras * 32, MPI_CHAR, 0, comm);
    for (int i = 0; i < num_cifras; i++) {
      cifras[i] = &cbloco[i * 32];
      std::string cifra(cbloco[i*32], cbloco[i*32 + 16]);
      falta.insert(i);
      sais[i] = cifra.substr(0, 2);
    }
  }

  // Iniciar thread de sincronização entre MPI workers
  std::thread sync_thread;
  // if (mpi_size > 1) {
  //   sync_thread =
  //       std::thread(mpi_sync, mpi_rank, mpi_size, &comm);
  // }

  // Usar todos threads disponíveis
  int num_threads = omp_get_max_threads();
  // num_threads = 1;
  omp_set_num_threads(num_threads);
  fprintf(stderr, "p%d Usando %d threads\n", mpi_rank, num_threads);

  std::map<std::string, std::string> solucoes;
  unsigned long long i = 0L, counter = 0;
#pragma omp parallel reduction(+ : counter)
  {
    // Inicializar sais (aceleração grande)
    std::map<std::string, crypt_data> crypt_data_por_sal;
    for (auto &ss : sais) {
      crypt_data_por_sal[ss] = crypt_data();
    }
    crypt_data *crypt_pointer;
    char *result;
    int thread_rank = omp_get_thread_num();
    int inicio = (mpi_rank * omp_get_num_threads()) + omp_get_thread_num();
    int passo = mpi_size * omp_get_num_threads();
    fprintf(stderr, "p%d t%d inicia em %d (passo %d), existem %d threads\n",
            mpi_rank, thread_rank, inicio, passo, omp_get_num_threads());
    Senha senha(inicio);
    unsigned long long thread_i;
    std::set<int> thread_falta(falta);
    for (thread_i = inicio; thread_i < maximo && !stop; thread_i += passo) {
      if ((falta.size() < thread_falta.size())) {
// #pragma omp barrier
#pragma omp critical(falta_global)
        { thread_falta = falta; }
        if ((int)thread_falta.size() == 0) {
          break;
        }
      }
      for (auto &e : thread_falta) {
        // printf("p%d t%d %s %s\n", mpi_rank, thread_rank, cifras[e],
        //        senha.getSenha());
        crypt_pointer = &(crypt_data_por_sal[sais[e]]);
        result = crypt_r(senha.getSenha(), cifras[e], crypt_pointer);
        int ok = strncmp(result, cifras[e], 14) == 0;

        if (ok) {
          // printf("p%*d, t%*d @ %2.f%%: %s = %s\n",
          // (int)ceil(log10(mpi_size)),
          //        mpi_rank, (int)ceil(log10(passo)), thread_rank,
          //        (thread_i / (double)maximo) * 100, cifras[e],
          //        senha.getSenha());
          printf("%s %s\n", cifras[e], senha.getSenha());
          fflush(stdout);
          // solucoes[cifras[e]] = senha.getSenha();

#pragma omp critical(falta_global)
          if (falta.count(e) > 0) {
            falta.erase(e);
          }
        }
      }

      if (((thread_i + 1) % 50000) == 0) {
        fprintf(stderr, "Realizado %2.f%% ou %llu de %llu\n",
                (thread_i / (double)maximo) * 100, thread_i + 1, maximo);
      }

      senha.prox(passo);
      counter++;
    }
#pragma omp barrier
  }
  stop = true;
  // if (mpi_size > 1) {
  //   sync_thread.join();
  // }
  fprintf(stderr, "[%d] terminou em %llu iterações!!!!\n", mpi_rank, counter);
  // for (auto &e : solucoes) {
  //   printf("%s %s\n", e.first.data(), e.second.data());
  // }

  MPI_Finalize();

  return 0;
}
