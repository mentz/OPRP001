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

// #include "crypt/crypt_r.h"

#define WAIT_TIME 100

int stop = 0;
int num_cifras = 0;
std::set<int> falta = std::set<int>();
int mpi_rank;
int mpi_size;
MPI_Comm comm = MPI_COMM_WORLD;
int falta_size = 0;

void mpi_master_relay() {
  std::set<int> done;
  int next_done;
  MPI_Request request;
  MPI_Status status;

  fprintf(stderr, "P%d iniciando mpi_master_relay\n", mpi_rank);

  // Receber notificação de cifra K quebrada,
  //   broadcast de K para todos workers.
  while (!stop) {
    int flag;
    MPI_Irecv(&next_done, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &request);
    sleep_for(WAIT_TIME);
    MPI_Test(&request, &flag, &status);
    if (flag) {
      done.insert(next_done);

// Processar a lista para que o master também retire os prontos
#pragma omp critical(falta_global)
      {
        if (falta.count(next_done) > 0)
          falta.erase(next_done);
        falta_size = falta.size();
      }

      fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);

      // Replicar para os workers
      MPI_Bcast(&next_done, 1, MPI_INT, 0, comm);
    }
  }
}

void mpi_worker_listener() {
  // Seção de sincronização de progresso
  std::set<int> done;
  int next_done;
  MPI_Request request;
  MPI_Status status;

  fprintf(stderr, "P%d iniciando mpi_worker_listener\n", mpi_rank);

  while (falta.size() > 0 && !stop) {
    int flag;
    // Receber int K do broadcast do root
    //   adicionar esse int K no set done.
    MPI_Ibcast(&next_done, 1, MPI_INT, 0, comm, &request);
    sleep_for(WAIT_TIME);
    MPI_Test(&request, &flag, &status);
    if (flag) {
// Processar a lista para que o master também retire os prontos
#pragma omp critical(falta_global)
      {
        if (falta.count(next_done) > 0)
          falta.erase(next_done);

        falta_size = falta.size();
      }

      fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);
    }
  }
}

void force_stop(int signal) {
  fprintf(stderr, "Encerramento forçado: sinal %d\n", signal);
  stop = 1;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, force_stop);

  int thread_level;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &thread_level);

  mpi_rank = 0;
  mpi_size = 1;
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
    fprintf(stderr, "Falta argumento: %s <comprimento_maximo> [início]\n",
            argv[0]);
    fprintf(stderr, "Uso: Informe pela entrada padrão o número de cifras, "
                    "número de threads  e em "
                    "seguida digite\n");
    fprintf(stderr, "     as cifras uma por linha.\n");
    exit(1);
  }

  // Ler senhas e sincronizar com outros processos MPI
  num_cifras = 0;
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
    std::vector<std::string> vec_cifras;
    for (int i = 0; i < num_cifras; i++) {
      getline(std::cin, cifra);
      falta.insert(i);
      vec_cifras.push_back(cifra);
    }
    std::sort(vec_cifras.begin(), vec_cifras.end());
    for (int i = 0; i < num_cifras; i++) {
      cifras[i] = &cbloco[i * 32];
      strncpy(cifras[i], vec_cifras[i].data(), 16);
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
      std::string cifra(cbloco[i * 32], cbloco[i * 32 + 16]);
      falta.insert(i);
      sais[i] = cifra.substr(0, 2);
    }
  }

  falta_size = falta.size();

  // Iniciar thread de sincronização entre MPI workers
  std::thread *sync_thread;
  if (mpi_size > 1) {
    if (mpi_rank == 0)
      sync_thread = new std::thread(mpi_master_relay);
    else {
      sync_thread = new std::thread(mpi_worker_listener);
    }
  }

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
    // std::map<std::string, crypt_des_data> crypt_data_por_sal;
    for (auto &ss : sais) {
      crypt_data_por_sal[ss] = crypt_data();
      // crypt_data_por_sal[ss] = crypt_des_data();
    }
    crypt_data *crypt_pointer;
    // crypt_des_data *crypt_pointer;
    char *result;
    int thread_rank = omp_get_thread_num();
    int inicio = (mpi_rank * omp_get_num_threads()) + omp_get_thread_num();
    int passo = mpi_size * omp_get_num_threads();
    fprintf(stderr, "p%d t%d inicia em %d (passo %d), existem %d threads\n",
            mpi_rank, thread_rank, inicio, passo, omp_get_num_threads());
    Senha senha(inicio);
    unsigned long long thread_i;
    std::set<int> thread_falta(falta);
    int thread_falta_size = num_cifras;
    for (thread_i = inicio; thread_i < maximo && !stop; thread_i += passo) {
      // if ((falta.size() < thread_falta.size())) {
      if (falta_size < thread_falta_size) {

#pragma omp critical(falta_global)
        thread_falta = falta;

        thread_falta_size = thread_falta.size();
      }
      for (auto &e : thread_falta) {
        // printf("p%d t%d %s %s\n", mpi_rank, thread_rank, cifras[e],
        //        senha.getSenha());
        crypt_pointer = &(crypt_data_por_sal[sais[e]]);
        result = crypt_r(senha.getSenha(), cifras[e], crypt_pointer);
        // result = crypt_des(senha.getSenha(), cifras[e], crypt_pointer);
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
          int next_done = e;

          if (mpi_rank == 0 || mpi_size == 1) {
#pragma omp critical(falta_global)
            if (falta.count(e) > 0) {
              falta.erase(e);
            }

            // Replicar para os workers
            MPI_Bcast(&next_done, 1, MPI_INT, 0, comm);
          } else {
            MPI_Send(&next_done, 1, MPI_INT, 0, 0, comm);
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
  if (mpi_size > 1) {
    sync_thread->join();
    delete sync_thread;
  }
  fprintf(stderr, "[%d] terminou em %llu iterações!!!!\n", mpi_rank, counter);
  // for (auto &e : solucoes) {
  //   printf("%s %s\n", e.first.data(), e.second.data());
  // }

  MPI_Finalize();

  return 0;
}
