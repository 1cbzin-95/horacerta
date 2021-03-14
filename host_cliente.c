#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXIMOMSG 500

void enviarLogin(char buffer[],int tamanho,int meusocket);
void enviarSenha(char buffer[],int tamanho,int meusocket);
void diretorioAtual(char buffer[],int tamanho,int meusocket);
void navEntreDiretorio(char buffer[],int tamanho,int meusocket);
void listarArquivos(char buffer[],int tamanho,int meusocket);
void finalizarConnection(char buffer[],int tamanho,int meusocket);
void solicitarAbertura(char buffer[],int tamanho,int meusocket);

int main(int argc, const char *argv[]){
    char dominio[501];const char *d = dominio;/* Por motivo da função  gethostbyname receber como parametro (const char *name)*/
    printf("Informe um dominio para conectar via FTP: ");
    scanf("%[a-zA-Z.^' ']",dominio);
    getchar();
    struct hostent *host_info;
    struct in_addr *address;
    if (strlen(*&d) < 2){
        printf("Uso: %s <hostname>\n",argv[0]);
        exit(1);
    }
    // obtendo o IP do host passado, consulta ao servidor DNS
    host_info = gethostbyname(d);
    if (host_info == NULL){
        printf("Nao pude encontrar %s\n",*&d);
        return 0;
    }
    else{
        address = (struct in_addr *) (host_info->h_addr);
        // printf("%s tem endereco: %s\n", *&d, inet_ntoa(*address));
        printf("Conectado ao servidor %s (%s) na porta 21\n",*&d,inet_ntoa(*address));
    }
    char buffer[MAXIMOMSG + 1]; // para incluir o terminador nulo
    int tamanho, meusocket;
    struct sockaddr_in destinatario;
    meusocket = socket(AF_INET, SOCK_STREAM, 0);
    destinatario.sin_family = AF_INET;
    // inet_ntoa(*address) tem o ip convertido(dns)
    destinatario.sin_addr.s_addr = inet_addr(inet_ntoa(*address));
    // porta do servidor | porta 21 por causa do Ftp
    destinatario.sin_port = htons(21);
    // zerando o resto da estrutura
    memset(&(destinatario.sin_zero), '\0', sizeof(destinatario.sin_zero));
    connect(meusocket, (struct sockaddr *)&destinatario, sizeof(struct sockaddr));
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    // terminando a mensagem
    buffer[tamanho] = '\0';
    printf("Mensagem do servidor:%s\n", buffer);
    enviarLogin(buffer,tamanho,meusocket);
    enviarSenha(buffer,tamanho,meusocket);
    diretorioAtual(buffer,tamanho,meusocket);
    navEntreDiretorio(buffer,tamanho,meusocket);
    diretorioAtual(buffer,tamanho,meusocket);

    solicitarAbertura(buffer,tamanho,meusocket);
    //listarArquivos(buffer,tamanho,meusocket);
    finalizarConnection(buffer,tamanho,meusocket);

    // termina o socket
    close(meusocket);
    return 0;
}
void enviarLogin(char buffer[],int tamanho,int meusocket){
    char login[30];
    do{
    printf("Informe o Login:");
    scanf("%[a-zA-Z^' ']",login);
    getchar();
    //Mandando USER-- e recebendo resp do loguin
    char User[50]="USER ";
    strcat(User,login);
    strcat(User,"\r\n");
    strcpy(buffer,User);
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    }while((strstr(buffer,"331"))==NULL);//Se for nullo não tem esse codigo na resposta buffer/repita
}
void enviarSenha(char buffer[],int tamanho,int meusocket){
    char senha[30];
    do{
        if(strstr(buffer,"503")||strstr(buffer,"530")){
            printf("Mensagem do servidor para passord:%s\n", buffer);
            enviarLogin(buffer,tamanho,meusocket);
        }
        printf("Informe o Password:");
        scanf("%[a-zA-Z^' ']",senha);
        getchar();
        //Mandando PASS-- e recebendo resp da password
        char User[50]="PASS ";
        strcat(User,senha);//concatenando strings
        strcat(User,"\r\n");
        strcpy(buffer,User);
        send(meusocket, buffer, strlen(buffer), 0);
        tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
        buffer[tamanho] = '\0';
    }while((strstr(buffer,"230"))==NULL);//Se for nullo não tem esse codigo na resposta buffer/repita
}
void diretorioAtual(char buffer[],int tamanho,int meusocket){
    //Mandando PWD-- descobrindo diretorio atual
    strcpy(buffer,"PWD\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("O diretorio Atual é:%s\n", buffer);
}
void navEntreDiretorio(char buffer[],int tamanho,int meusocket){
    //Mandando CWD-- NavegaNDO ENTRE OS diretorios
    strcpy(buffer,"CWD /pub/example\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("mudando para :%s\n", buffer);
}
void finalizarConnection(char buffer[],int tamanho,int meusocket){
    //Mandando QuiT-- Terminando conecção
    strcpy(buffer,"QUIT\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("Mensagem do Servidor:%s\n", buffer);
}
void solicitarAbertura(char buffer[],int tamanho,int meusocket){
    strcpy(buffer,"PASV\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("PASV:%s\n", buffer);                                                   // pegar os penultimos caracteres, transformar em numero
    // if((strstr(buffer,"150 Opening ASCII mode data connection"))!=NULL){        // calcular a porta de buscar os arquivo
                                                                                   //
    // }
}
void listarArquivos(char buffer[],int tamanho,int meusocket){
    strcpy(buffer,"LIST\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("tem:%s\n", buffer);
}

/*referencias 
    https://www.gta.ufrj.br/ensino/eel878/sockets/gethostbynameman.html
*/