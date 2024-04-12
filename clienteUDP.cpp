#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h> // Para medir o tempo
#include <fstream> // Para escrever no arquivo CSV

int main() {
    int sockfd;
    int len;
    socklen_t len_recv;
    struct sockaddr_in address;
    int result;
    char vetor_ch[4] = {'A', 'B', 'C', 'D'};
    
    unsigned short porta = 9734;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // criação do socket
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(porta);
    
    len = sizeof(address);
    len_recv = sizeof(address);

    // Variáveis para calcular a média do tempo
    double total_time = 0.0;
    double average_time;

    // Abrindo arquivo CSV para escrever os tempos
    std::ofstream outfile("tempos.csv");
    if (!outfile.is_open()) {
        perror("Erro ao abrir o arquivo CSV");
        exit(1);
    }
    outfile << "Amostra,Tempo (ms)" << std::endl;

    // Enviar e capturar tempo 30 vezes
    for (int i = 1; i <= 30; ++i) {
        struct timeval start, end;
        
        printf("Enviando mensagem %d para o servidor\n", i);

        gettimeofday(&start, NULL); // Captura o tempo de início
        
        sendto(sockfd, vetor_ch, sizeof(vetor_ch), 0, (struct sockaddr *)&address, len);
        recvfrom(sockfd, vetor_ch, sizeof(vetor_ch), 0, (struct sockaddr *)&address, &len_recv);
        
        gettimeofday(&end, NULL); // Captura o tempo de fim
        
        // Calcula o tempo decorrido em milissegundos
        double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0; // segundos para milissegundos
        elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0; // microssegundos para milissegundos

        total_time += elapsed_time;

        // Escreve o número da amostra e o tempo no arquivo CSV
        outfile << i << "," << elapsed_time << std::endl;
    }

    // Calcula a média do tempo
    average_time = total_time / 30;

    printf("Tempo médio de comunicação: %.2f ms\n", average_time);

    // Fecha o arquivo CSV
    outfile.close();

    close(sockfd);
    exit(0);
}

