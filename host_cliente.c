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

int main(int argc, const char *argv[]){
    char dominio[501];const char *d = dominio;/* Por motivo da função  gethostbyname receber como parametro (const char *name)*/

    printf("Informe um dominio para conectar via FTP: ");
    scanf("%[a-zA-Z.^' ']",dominio);
    getchar();
    struct hostent *host_info;
    struct in_addr *address;

    if (strlen(*&d) < 2){ //se não tiver nada fora '/0'
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
        printf("Conectado ao servidor %s (%s) na porta 21 ... \n",*&d,inet_ntoa(*address));
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

/* Dividir que se repete e transformar em função*/

    //Mandando USER-- e recebendo resp do loguin
    strcpy(buffer,"USER demo\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("Mensagem do servidor para login:%s\n", buffer);

    //Mandando PASS-- e recebendo resp da password
    strcpy(buffer,"PASS password\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("Mensagem do servidor para passord:%s\n", buffer);

    //Mandando PWD-- descobrindo diretorio atual
    strcpy(buffer,"PWD\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("O diretorio Atual é:%s\n", buffer);

    //Mandando CWD-- NavegaNDO ENTRE OS diretorios
    strcpy(buffer,"CWD /pub/example\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("mudando para :%s\n", buffer);

    //Mandando PWD-- descobrindo diretorio atual
    strcpy(buffer,"PWD\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("O diretorio Atual é:%s\n", buffer);

    //Mandando PWD-- descobrindo diretorio atual
    strcpy(buffer,"QUIT\r\n");
    send(meusocket, buffer, strlen(buffer), 0);
    tamanho = recv(meusocket, buffer, MAXIMOMSG, 0);
    buffer[tamanho] = '\0';
    printf("Mensagem do Servidor:%s\n", buffer);


    // termina o socket
    close(meusocket);

    return 0;
}









//teste.rebex.net
    // inet_ntoa(*address) -> tem o endereço
    //./host test.rebex.net
    // ./host labepi.ufrn.br 21

    //--------------------------------------------------Peguei o ip
    /**
    * programa cliente
    */


/*
Description of data base entry for a single host. 
struct hostent
{
  char *h_name;			// Official name of host. 
  char **h_aliases;		// Alias list. 
  int h_addrtype;		// Host address type. 
  int h_length;			// Length of address. 
  char **h_addr_list;	// List of addresses from name server. 
#if defined __USE_MISC || defined __USE_GNU
# define	h_addr	h_addr_list[0] //q Address, for backward compatibility.
#endif
};

*/

/*referencias 
    https://www.gta.ufrj.br/ensino/eel878/sockets/gethostbynameman.html
*/