#include "chat.hpp"

void print_buffer() {

  if (my_contact.contacts[currentContact].messages.size()>0)
  {
    printf ("Mensagens Anteriores\n");

    for (int i=0; i<my_contact.contacts[currentContact].messages.size(); i++)
    {
      cout << "Mensagem de " << my_contact.contacts[currentContact].name << ":" << my_contact.contacts[currentContact].messages[i];
    }

    my_contact.contacts[currentContact].messages.clear();
  }
}

void print_contacts() {

  printf ("Selecione um dos contatos abaixo: \n");

  for (int i=0; i<my_contact.contacts.size(); i++)
  {
	cout << i <<"\t" << my_contact.contacts[i].name << "\t" << my_contact.contacts[i].IP <<"\n";
  }
}

void group_message() {

	int n, select=0;

	while(select!=-1)
	{	
		system("clear");
		printf ("Selecione o contato (digite -1 para terminar a seleçao): \n");

		for (int i=0; i<my_contact.contacts.size(); i++)
		{
			if(group[i] == 0)
			{
				cout << i <<"\t" << my_contact.contacts[i].name << "\t" << my_contact.contacts[i].IP <<"\n";
			}
		}

		scanf("%d", &select);
		fflush(stdin);
        scanf("%*c");

		group[select] = 1;
	}

	system("clear");
	bzero(buffer_write, BUFF_SIZE);
    fflush(stdin);
    printf ("Mensagem: ");
    fgets(buffer_write,BUFF_SIZE,stdin);

	for (int i=0; i<my_contact.number_friends; i++)
	{
		if(group[i] == 1)
		{
			n = write(my_contact.contacts[i].socket, buffer_write, strlen(buffer_write));
      group[i] = 0;
		}
	}
}

void send_message() {

	int n, cont, flag=1;

	system("clear");

	if (my_contact.contacts.size()==0)
	{
		printf ("Nenhum contato :( \n");

		return;
	}

  while(flag) {
    
    system("clear");
    print_contacts();
    fflush(stdin);
    scanf("%d", &currentContact);
    scanf("%*c");
    
    if (currentContact < my_contact.contacts.size())
    {
        flag=0;
    }
    
    system("clear");

  }

  printf ("Digite *SAIR para voltar ao menu !\n");

  print_buffer();

   	while(1) {

   		bzero(buffer_write, BUFF_SIZE);
      	fflush(stdin);
    	fgets(buffer_write,BUFF_SIZE,stdin);

      stringstream aux;
      aux << "*SAIR" << "\n";

      if (buffer_write == aux.str())
      {
        system("clear");
        break;
      }

    	n = write(my_contact.contacts[currentContact].socket, buffer_write, strlen(buffer_write));

    	if (n == -1){
    		break;
    	}

   	}
}

void receive_message() {

	int n, i;

   	while(1) {

   		pthread_mutex_lock (&mutex);

   		for(i=0; i<my_contact.contacts.size(); i++)
   		{
        	bzero(buffer_read, BUFF_SIZE);
   			n = read(my_contact.contacts[i].socket, buffer_read, BUFF_SIZE);

   			if (isclosed(my_contact.contacts[i].socket))
   			{
   				remove_client(i);
   			}


   			if(n>0 && i!=currentContact)
   			{
          	my_contact.contacts[i].messages.push_back(buffer_read);
            
            /*OPCIONAL !!*/

            //stringstream aux;

            //aux << "notify-send 'Nova mensagem de "<< my_contact.contacts[i].name << "' " << "'" << buffer_read << " '";

            //system(aux.str().c_str());

            /*OPCIONAL !!*/
   			}
        	else if (n>0 && i==currentContact)
        	{
          	cout << my_contact.contacts[currentContact].name <<": " << buffer_read;
        	}
   		}		

   		pthread_mutex_unlock (&mutex);
   }
}   

void create_server() {

	int sockfd, newsockfd, clilen;     //Dois primeiros são descritores de arquivos (Retornados pela socket SYSCALL e Accept SYSCALL)
	struct sockaddr_in serv_addr;			         //Struct de endereço se internet e endereço de server e cliente respectivamente
    my_contact.port_number = PORTNO;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);      //Cria novo socket: 3 argumentos:
                                                   // 1 - address domain of the socket (internet domain)
                                                   // 2 - type of socket (stream socket (TCP))
                                                   // 3 - protocolo (DEVE SER ZERO), neste caso será TCP
                                                   //Retorna um ponteiro de arquivo para o socket, ou -1 caso de errado
     if (sockfd < 0) 
        printf("\nERRO AO CRIAR O SOCKET SERVIDOR\n");

     bzero((char *) &serv_addr, sizeof(serv_addr)); //Zera o valor de serv_addr

     serv_addr.sin_family = AF_INET;                    //Código para endereço de família, deve ficar assim
     serv_addr.sin_addr.s_addr = INADDR_ANY;            //Endereço IP do host. Seta para o da máquina
     serv_addr.sin_port = htons(PORTNO);                //Número da porta, deve ser convertido para "internet order"
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,   //Praticamente cria o socket, usando numero de porta e endereço: 3 argumentos: 
              sizeof(serv_addr)) < 0){                  // 1 - Socket file descriptor
              printf("\nERRO ao criar socket\n");       // 2 - Endereço
          	  exit (1);
         }
                                                        // 3 - Tamanho do endereço
                                                        //Deve fazer cast de sockaddr_in para sockeaddr
     listen(sockfd,5);                                  // SYSCALL que permite ao processo fazer ralizar conexões pelo socket
                                                        //Dois argumentos:
                                                        // 1 - Ponteiro para socket criado
                                                        // 2 -  the number of connections that can be waiting while the process is handling a particular connection
                                                        //Geralmente 5s 
    my_contact.socket_init_pt = sockfd;
}

void accept_connection() {

	while(1) {

	struct sockaddr_in cli_addr;     //Struct de endereço se internet e endereço de server e cliente respectivamente
	int sockfd;
	struct hostent* server;
	stringstream aux;
  
   unsigned int clilen = sizeof(cli_addr);                                             //Tamanho do endereço do cliente

    sockfd = accept(my_contact.socket_init_pt, (struct sockaddr *) &cli_addr, &clilen);   //Processo bloqueia até um cliente se conectar ao server
    																					  //Quando a conexão é bem sucedida retorna um novo file descriptor

    if (sockfd > 0)	                                            				         
    {
                                     				 //Deve-se usar o novo socket, parametros são endereços dos clientes

        fcntl(sockfd, F_SETFL, O_NONBLOCK);

        s_friend auxContact;                                          // Turns the new socket into a non-blocking socket

        auxContact.socket = sockfd;

        aux << "Contato " << my_contact.number_friends;

        auxContact.name = aux.str();

        auxContact.IP = inet_ntoa(cli_addr.sin_addr);

        my_contact.contacts.push_back(auxContact);

        my_contact.number_friends++;

        /*OPCIONAL !!*/

        //stringstream aux;

        //aux << "notify-send 'Conexão estabelecida !' 'Um novo contato foi adicionado ! Contato " << my_contact.number_friends-1 << " !'";

        //system(aux.str().c_str());

        /*OPCIONAL !!*/
    }

	}

}

void create_client() {

	int sockfd, n;      //Ponteiro para socket, numero de porta e valor retornado por readou write

    struct sockaddr_in serv_addr;   //Struct para conter o endereço do server
    struct hostent *server;         //Ponteiro para struct do tipo hostnet
   	char server_name[512];
   	string name, IP;  

    printf("DIGITE O IP DO HOST O QUAL DESEJA SE CONECTAR: \n");
   	scanf("%s", server_name);
   	getchar();

   	IP = server_name;

	  printf("DIGITE O NOME DO CONTATO: \n");
	  cin >> name;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);       //Cria socket

    if (sockfd < 0)                                 //Caso de erro
        printf("\nERRO ao abrir o socket \n");
  
    server = gethostbyname(server_name);                //Retorna um ponteiro com informações do host, como se fosse um DNS
                                                    //The field char *h_addr contains the IP address.
  
    if (server == NULL) {                           //Caso não ache o host
        fprintf(stderr,"\nERRO, host nao existe \n");
		return;        
    }
  
    bzero((char *) &serv_addr, sizeof(serv_addr));      //Zera struc de endereço de server
  
    serv_addr.sin_family = AF_INET;             //Define entrada.....
  
    bcopy((char *)server->h_addr,               //Copia o endereço do server achado pelo DNS para a struct do
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
  
    serv_addr.sin_port = htons(PORTNO);         //Porta do server recebe as informações buscadas

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("\nERRO ao conectar\n");
        return;
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK); 

    s_friend auxContacts;

    auxContacts.socket = sockfd;

    auxContacts.name = name;

    auxContacts.IP = IP;

    my_contact.contacts.push_back(auxContacts);

    my_contact.number_friends++;   

    system("clear");

}

void remove_friend() {

	system("clear");
        
	pthread_mutex_lock (&mutex);

    int index;				// indice do contato a ser excluido

	print_contacts();

	scanf("%d", &index);

	close(my_contact.contacts[index].socket);

	my_contact.contacts.erase(my_contact.contacts.begin() + index);

	pthread_mutex_unlock (&mutex);
	
}

void remove_client(int index) {

	close(my_contact.contacts[index].socket);

	my_contact.contacts.erase(my_contact.contacts.begin() + index);
}

void name_contact() {
	
	int i;
	string name;

	system ("clear");

	print_contacts();

 	scanf ("%d", &i);

 	cout << "Digite o novo nome para " << my_contact.contacts[i].name << ": \n";
 	cin >> name;

 	my_contact.contacts[i].name = name;
}

bool isclosed(int sock) {
  
  fd_set rfd;
  FD_ZERO(&rfd);
  FD_SET(sock, &rfd);
  
  timeval tv = { 0 };
  
  select(sock+1, &rfd, 0, 0, &tv);
  
  if (!FD_ISSET(sock, &rfd))
    return false;
  
  int n = 0;
  
  ioctl(sock, FIONREAD, &n);
  
  return n == 0;
}

int main(int argc, char * argv[]) {

  int cmd = 0;

  printf(" 　　　　/)―ヘ\n");    
  printf("　　＿／　  　　＼\n");
  printf("／　　　  ●　　●丶\n");
  printf("｜　　　   　　▼　|\n");
  printf("｜   　　　　　亠ノ\n");
  printf(" U￣U￣￣￣U\n");

  /*printf ("O servidor foi criado com sucesso !\nSeu Hostname é:");

  printf("Digite a função desejada: ");*/

  create_server();

  thread thread_accept_connection(accept_connection);
  thread thread_receive_message(receive_message);

  printf ("O Servidor foi Criado com sucesso !\n");
  printf ("Iniciando thread para aceitar conexão !\n");
  printf ("Iniciando thread para receber mensagens !\n");

  printf("\n\n");

  if (pthread_mutex_init(&mutex, NULL) != 0)
  {
  	printf("\nMutex init failed!\n");
  	exit(1);
  }

  while (cmd!=6)
  {  
    currentContact = -1;

    system ("clear");

    printf ("Digite o comando necessário: \n");

    printf(" 1 - Adicionar contato\n");
    printf(" 2 - Remover contato\n");
    printf(" 3 - Selecionar contato\n");
    printf(" 4 - Mensagem em grupo\n");
    printf(" 5 - Nomear um contato\n");
    printf(" 6 - Sair\n");

    scanf ("%d", &cmd);

    if (cmd == 1)
    {
        create_client();
    }
     
    else if (cmd == 2)
    {
        remove_friend();
    }

    else if (cmd == 3)
    {
        send_message();
    }

    else if (cmd == 4)
    {
        group_message();
    }
    
    else if(cmd == 5)
    {
        name_contact();
    }

    else if(cmd == 6)
    {
    	for (int i = 0; i < my_contact.contacts.size(); i++)
    	{
    		close(my_contact.contacts[i].socket);
    	}

    	close(my_contact.socket_init_pt);
    	
    	exit(1);
    }
  }
	
	return 0;
}
