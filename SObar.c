#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

/////////
//          Variáveis compartilhadas, filas e semáforos.
/////////

sem_t *pedidos, newRound, ca, cb, allServed, novo, servi, rodadaAtiva;
//pedidos:   || newRoud: controla o início de uma nova rodada || ca: controla o índice da fila de clientes que precisam pedir
//cb: Acesso ao indice da fila de clientes a serem atendidos || allServed: usado para definir se todos os garçoms serviram seus clientes
//servi: controla a variável serviTodos, atualizada quando garçom termina de servir sua fila || ClientIDs: controla a ID individual do cliente || WaiterIDs: ID individual do garçom
bool fechouBar = false, existemClientesNoBar = true;
int rodada,roundsTillNow, serviTodos;


int idCliente,clientesPedidos;
int indicePedClientes,indicePedGarcoms;

volatile int *querTrago, *estadoClientes; //Fila de clientes que querem fazer o pedido..

//Parâmetros: Num. de Clientes; Num de Garçoms; Capacidade de atendimento dos garçoms; Rodadas grátis.
int clients,waiters,C,rounds;

////////////////////////////////////////////////////////////
//                         CLIENTES                   //////
////////////////////////////////////////////////////////////

void fazPedido(int id)
{

    //Já bebeu nesta rodada, nao pode fazer seu pedido ainda.
    while(estadoClientes[id] == 1);

    sem_wait(&ca);

    querTrago[indicePedClientes] = id;
    indicePedClientes++;
    if(indicePedClientes==clients)  // Implementação de uma lista circular com o vetor.
        indicePedClientes = 0;

    sem_post(&ca);
    printf("Cliente %d pediu trago..\n",id);



}
void esperaPedido(int id)
{
    sem_wait(&pedidos[id]);

}
void recebePedido(int id)
{
    estadoClientes[id]= 1; // Sinaliza que este cliente vai beber nesta rodada.
    printf("Cliente %d: 且ヽ(^O^*ヽ)   \n",id);

}
void consomePedido()
{
    sleep(rand()%10);
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                  GARÇOMS                                     /////////
/////////////////////////////////////////////////////////////////////////////////////////////
void recebeMaximoPedidos(int *fila, int id)
{

    printf("Garçom %d esperando pedidos\n",id);
    int orders = 0,aux = 0,i = 0; //Orders = pedidos até agora, aux = indice da fila do garçom;

   // printf("%d | %d \n",indicePedGarcoms,querTrago[indicePedGarcoms]);

    while(orders < C)  // Enquanto nao atender o max de clientes possivel.
    {

        sem_wait(&cb); // Acesso ao indice do prox cliente a ser atendido.

        if(querTrago[indicePedGarcoms] != -1)
        {
            //Armazena o indice que deve atender e atualiza o indice do próximo;
            i = indicePedGarcoms;
            indicePedGarcoms++;
            if(indicePedGarcoms==clients)
                indicePedGarcoms=0;

            sem_post(&cb); // Libera o indice para o atendimento do prox cliente.

            fila[aux] = querTrago[i]; //Coloca id do cliente na fila para atendimento.
            aux++;
            orders++;
            printf("Pedido do cliente %d anotado \n",querTrago[i]);
        }
        else
        {

            sem_post(&cb); //Libera o indice sem atender ninguem.
        }

    }

}
void registraPedidos(int id)
{
    printf("GARÇOM %d vai a copa registrar os pedidos.\n",id);
    sleep(rand()%10);
    printf("Pedidos registrados: %d \n", id);
}


void entregaPedidos(int *fila,int id)
{
    int i,cl;
    for(i=0; i<C; i++)  // Percorre toda a fila de atendimento do garçom
    {
        if(fila[i] == -1) // Nao haviam clientes para ocupar toda a fila.
            break;
        //Entrega pedido do cliente na posição i;
        cl = fila[i];
        fila[i] = -1;
        printf("Pedido do cliente %d entregue \n",cl);
        sem_post(&pedidos[cl]); // Libera o pedido do cliente
    }

    sem_wait(&servi);
    printf("\n %d Serviu todos\n",id);
    serviTodos++;
    if(serviTodos == waiters) // Se todos os garçons terminaram de servir, libera allServed.
        sem_post(&allServed);
    sem_post(&servi);
}

//Valor inicial da fila de atendimento dos garçoms: -1;
void startFila(int *fila){
    int i;
    for(i = 0; i < C; i++){
        fila[i] = -1;
    }
}

//////////////////////////////////////////////////////////////////////////
// Reinicia tudo o que  necessario para o começo de uma nova rodada
void novaRodada()
{

    printf("\nNewRound\n\n");
    sem_wait(&allServed); // Espera os garçoms sinalizarem que serviram todos das suas filas.

    printf("\n\n PREPARANDO UMA NOVA RODADA! \n\n\n");
    sem_wait(&ca);
    sem_wait(&cb);

    printf("Direcionando os garçoms.\n");
    int i;
    if(indicePedGarcoms==0)
    {
        for(i=0; i<clients; i++) //Todos os clientes foram servidos, atualiza lista de bebados.
        {
            querTrago[i] = -1;
            estadoClientes[i] = 0;
        }
    }
    else
    {
        for(i=0; i<indicePedGarcoms-1; i++) // Alguns clientes nao conseguiram realizar seu pedido, serao os primeiros na proxima rodada.
        {
            estadoClientes[querTrago[i]] = 0;
            querTrago[i] = -1;

        }
    }

    sem_wait(&servi);
    serviTodos = 0;
    sem_post(&servi);

    sem_post(&ca);
    sem_post(&cb);

    printf("Conversando com o gerente. \n");
    roundsTillNow = rodada/waiters;

    if(roundsTillNow == rounds){
        printf("Acabaram as rodadas da casa.\n");
        fechouBar = true;
    }else{
        printf("\nNOVA RODADA LIBERADA!\n");
        printf("RODADA %d!\n",rodada);
        printf("\n\n");
    }

    for(i=0; i<clients+waiters; i++)
        sem_post(&newRound);

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void *cliente(int id)
{
    while (!fechouBar)
    {
        //if(estadoClientes[id]!=1){
            fazPedido(id);
            esperaPedido(id);
            recebePedido(id);
            consomePedido(id); //tempo variavel
        //}
        sem_wait(&newRound);
    }

    sem_wait(&ca);
    clients --;
    if(clients == 0){
        existemClientesNoBar = false;
        C = 0;
    }
    sem_post(&ca);
}


void *garcom(int id)
{
    int *filaDePedidos;
    filaDePedidos = malloc(C * sizeof(int));

    startFila(filaDePedidos);

    while(existemClientesNoBar)
    {

        recebeMaximoPedidos(filaDePedidos,id);
        registraPedidos(id);
        entregaPedidos(filaDePedidos,id);
        rodada++; // serve como param p/ fechar o bar

        if(id == 0)
            novaRodada(); // Apenas um garçom precisa identificar o começo de um novo round.

        sem_wait(&newRound);
    }

}

void iniciaSemaforos()
{

    int i,total;
    sem_init(&ca,0,1);
    sem_init(&cb,0,1);
    sem_init(&allServed,0,0);

    sem_init(&servi,0,1);

    sem_init(&newRound,0,0);


    for(i=0; i<clients; i++)
    {
        sem_init(&pedidos[i],0,0);
    }

}

void iniciaControleDoBar(){

    indicePedClientes = 0;
    indicePedGarcoms = 0;
    fechouBar = false;
    existemClientesNoBar = true;

    serviTodos = 0;
}


int main(int argc, char *argv[])
{

    printf("Started \n");
    if(argc < 5)
    {
        printf("Use ./exec Clientes Garçoms Clientes-por-garçom Rodadas\n");
        exit(1);
    }

    clients = atoi(argv[1]);
    waiters = atoi(argv[2]);
    C = atoi(argv[3]);
    rounds = atoi(argv[4]);


    rodada = 0;
    //Alocação dos semáforos dos clientes; Indica o estado: 0 = esperando; 1 = recebeu o pedido.
    pedidos = malloc (clients * sizeof(sem_t));


     printf("Limpando as mesas.\n");
    iniciaSemaforos();
    //Seed para o gerador aleatório
    srand(time(NULL));

    printf("Clientes chegando.\n");
    //Alocação do vetor de clientes, sinalizando que querem fazer o pedido;
    querTrago = malloc (clients * sizeof(int));
    //Vetor de estado de todos os clientes do bar;
    estadoClientes = malloc (clients * sizeof(int));

    //Inicialização das variveis de controle do bar;
    iniciaControleDoBar();

    printf("Garçoms se preparando.\n");

    int i,rc;
    int *clientID,*waiterID;
    clientID = malloc(clients * sizeof(int));
    waiterID = malloc(waiters * sizeof(int));
    //Ids dos garçoms e dos clientes, respectivamente.
    for(i = 0; i < waiters; i++){
        waiterID[i] = i;
    }
    for(i = 0; i < clients; i++){
         clientID[i] = i;
    }

    pthread_t *clientes,*garcoms;
    clientes = malloc(clients * sizeof(pthread_t));
    garcoms = malloc(waiters * sizeof(pthread_t));

    printf("Bar Aberto!\n");
    for(i = 0; i < clients; i++)
    {

        rc = pthread_create(&clientes[i], NULL, cliente,(int*)clientID[i]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create()is %d \n", rc);
            exit(-1);
        }
        //   printf("IN FOR\n");
    }

    for(i = 0; i < waiters; i++)
    {

        rc = pthread_create(&garcoms[i], NULL, garcom, (int*)waiterID[i]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create()is %d \n", rc);
            exit(-1);
        }
        //   printf("IN FOR\n");
    }

    pthread_join(clientes[1], NULL);

}
