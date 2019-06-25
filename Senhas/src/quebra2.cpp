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

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define WAIT_TIME 15

int stop = 0;
int num_cifras = 0;
std::set<int> falta = std::set<int>();
int mpi_rank;
int mpi_size;
MPI_Comm comm = MPI_COMM_WORLD;
int falta_size = 0;

// void mpi_master_relay() {
//   std::set<int> done;
//   int next_done;
//   MPI_Request request;
//   MPI_Status status;

//   fprintf(stderr, "P%d iniciando mpi_master_relay\n", mpi_rank);

//   // Receber notificação de cifra K quebrada,
//   //   broadcast de K para todos workers.
//   while (!stop) {
//     int flag;
//     MPI_Irecv(&next_done, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &request);
//     do {
//       sleep_for(WAIT_TIME);
//       MPI_Test(&request, &flag, &status);
//     } while (!stop && !flag);
//     if (flag) {
//       done.insert(next_done);

// // Processar a lista para que o master também retire os prontos
// #pragma omp critical(falta_global)
//       {
//         if (falta.count(next_done) > 0)
//           falta.erase(next_done);
//         falta_size = falta.size();
//       }

//       fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);

//       // Replicar para os workers
//       MPI_Ibcast(&next_done, 1, MPI_INT, 0, comm, &request);
//       MPI_Test(&request, &flag, &status);
//       do {
//         sleep_for(WAIT_TIME);
//         MPI_Test(&request, &flag, &status);
//       } while (!stop && !flag);
//       if (stop) {
//         // I must die :(
//         MPI_Cancel(&request);
//         MPI_Wait(&request, &status);
//         // flag = false;
//         // do {
//         //   sleep_for(WAIT_TIME);
//         //   MPI_Test_cancelled(&status, &flag);
//         // } while (!flag);
//       }
//     } else {
//       // I must die :(
//       MPI_Cancel(&request);
//       MPI_Wait(&request, &status);
//       // flag = false;
//       // do {
//       //   sleep_for(WAIT_TIME);
//       //   MPI_Test_cancelled(&status, &flag);
//       // } while (!flag);
//     }
//   }
// }

// void mpi_worker_listener() {
//   // Seção de sincronização de progresso
//   std::set<int> done;
//   int next_done;
//   MPI_Request request;
//   MPI_Status status;

//   fprintf(stderr, "P%d iniciando mpi_worker_listener\n", mpi_rank);

//   while (falta.size() > 0 && !stop) {
//     int flag;
//     // Receber int K do broadcast do root
//     //   adicionar esse int K no set done.
//     MPI_Ibcast(&next_done, 1, MPI_INT, 0, comm, &request);
//     do {
//       sleep_for(WAIT_TIME);
//       MPI_Test(&request, &flag, &status);
//     } while (!stop && !flag);
//     if (flag) {
// // Processar a lista para que o master também retire os prontos
// #pragma omp critical(falta_global)
//       {
//         if (falta.count(next_done) > 0)
//           falta.erase(next_done);

//         falta_size = falta.size();
//       }

//       fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);
//     } else {
//       // I must die :(
//       MPI_Cancel(&request);
//       MPI_Wait(&request, &status);
//       // flag = false;
//       // do {
//       //   sleep_for(WAIT_TIME);
//       //   MPI_Test_cancelled(&status, &flag);
//       // } while (!flag);
//     }
//   }
// }

void force_stop(int signal) {
  fprintf(stderr, "Encerramento forçado: sinal %d\n", signal);
  stop = 1;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, force_stop);
  signal(SIGTERM, force_stop);
  signal(SIGUSR1, force_stop);
  signal(SIGUSR2, force_stop);

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
  std::map<std::string, int> sais;
  std::map<int, std::string> sal_por_cifra;
  std::vector<int> sal_por_indice;
  char **cifras;
  char *cbloco;
  if (mpi_rank == 0) {
    // ROOT
    std::cin >> num_cifras;
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    getchar();
    sal_por_indice = std::vector<int>(num_cifras);
    cifras = new char *[num_cifras];
    cbloco = new char[num_cifras * 32];
    std::string cifra;
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
      if (!sais.count(vec_cifras[i].substr(0, 2)))
        sais[vec_cifras[i].substr(0, 2)] = sais.size();
      sal_por_cifra[i] = vec_cifras[i].substr(0, 2);
      sal_por_indice[i] = sais[vec_cifras[i].substr(0, 2)];
    }
    MPI_Bcast(cifras[0], num_cifras * 32, MPI_CHAR, 0, comm);
  } else {
    // Not root
    MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    cifras = new char *[num_cifras];
    cbloco = new char[num_cifras * 32];
    sal_por_indice = std::vector<int>(num_cifras);
    MPI_Bcast(cbloco, num_cifras * 32, MPI_CHAR, 0, comm);
    for (int i = 0; i < num_cifras; i++) {
      cifras[i] = &cbloco[i * 32];
      std::string cifra(&cbloco[i * 32], &cbloco[i * 32 + 16]);
      falta.insert(i);
      if (!sais.count(cifra.substr(0, 2)))
        sais[cifra.substr(0, 2)] = sais.size();
      sal_por_cifra[i] = cifra.substr(0, 2);
      sal_por_indice[i] = sais[cifra.substr(0, 2)];
    }
  }

  // for (auto &e : sais) {
  //   fprintf(stderr, "%s %d\n", e.first.data(), e.second);
  // }
  // for (auto &e : sal_por_indice) {
  //   fprintf(stderr, "%d\n", e);
  // }

  falta_size = falta.size();

  // Iniciar thread de sincronização entre MPI workers
  // std::thread *sync_thread;
  // if (mpi_size > 1) {
  //   if (mpi_rank == 0)
  //     sync_thread = new std::thread(mpi_master_relay);
  //   else {
  //     sync_thread = new std::thread(mpi_worker_listener);
  //   }
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
    std::map<int, crypt_data> crypt_data_por_sal;
    // std::map<std::string, crypt_data> crypt_data_por_sal;
    // std::map<std::string, crypt_des_data> crypt_data_por_sal;
    for (auto &ss : sais) {
      crypt_data_por_sal[ss.second] = crypt_data();
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
      if (falta_size < thread_falta_size) {
#pragma omp critical(falta_global)
        thread_falta = falta;
        thread_falta_size = thread_falta.size();
      }

      for (auto &e : sais) {
        if (stop)
          break;
        crypt_pointer = &(crypt_data_por_sal[e.second]);
        result = crypt_r(senha.getSenha(), e.first.data(), crypt_pointer);
        // fprintf(stderr, "%-8s s%d = %s\n", senha.getSenha(), e.second,
        // result);
      }

      for (auto &e : thread_falta) {
        if (stop)
          break;
        // printf("p%d t%d %s %s\n", mpi_rank, thread_rank, cifras[e],
        //        senha.getSenha());
        crypt_pointer = &(crypt_data_por_sal[sal_por_indice[e]]);
        // int ok = strncmp(crypt_pointer->crypt_3_buf, cifras[e], 14) == 0;
        int ok = nstr_equals(crypt_pointer->crypt_3_buf, cifras[e], 14);
        // result = crypt_des(senha.getSenha(), cifras[e], crypt_pointer);
        // fprintf(stdout, "(%s == %s) %s\n", crypt_pointer->crypt_3_buf,
        //         cifras[e], (ok) ? "TRUE" : "");
        if (ok) {
          printf("%s %s\n", cifras[e], senha.getSenha());
          fflush(stdout);
          solucoes[cifras[e]] = senha.getSenha();
          int next_done = e;

#pragma omp critical(falta_global)
          if (falta.count(e) > 0) {
            falta.erase(e);
          }
        }
      }

      if (((thread_i + 1) % 2000000) == 0) {
        fprintf(stderr, "Realizado %2.f%% ou %llu de %llu\n",
                (thread_i / (double)maximo) * 100, thread_i + 1, maximo);
      }

      senha.prox(passo);
      counter++;
    }
#pragma omp barrier
  }
  // printf("p%d parei na barreira\n", mpi_rank);
  // MPI_Barrier(comm);
  // printf("p%d saí da barreira\n", mpi_rank);
  // stop = true;
  // if (mpi_size > 1) {
  //   sync_thread->join();
  //   delete sync_thread;
  // }
  fprintf(stderr, "[%d] terminou em %llu iterações!!!!\n", mpi_rank, counter);
  for (auto &e : solucoes) {
    printf("%s %s\n", e.first.data(), e.second.data());
  }

  MPI_Finalize();

  return 0;
}
