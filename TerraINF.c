//INCLUDES
#include<raylib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

//DEFINES
#define VDD 1
#define FALSO 0
#define X 0
#define Y 1
#define MATRIZ_LARGURA 30
#define MATRIZ_ALTURA 20
#define TILE_TAM 40
#define TELA_LARGURA 1200
#define TELA_ALTURA 800
#define COORDENADAS 2
#define MAX_OPCOES_MENU 3
#define ATIVADO 1
#define DESATIVADO -1

//ESTRUTURAS
typedef struct sprite
{
    Texture2D capa;
    Texture2D jogador;
    Texture2D jogador_escada;
    Texture2D jogador_picareta;
    Texture2D fundoTerra;
    Texture2D borda;
    Texture2D terra;
    Texture2D escada;
    Texture2D minerio;
    Texture2D hud;
    Texture2D gameOver;
}SPRITE_t;

typedef struct jogador
{
    int posX;
    int posY;
    int vida;
    int energia;
    int score;
    int numEscadas;
    int emCimaDaEscada;
    int modoMineracao;
    Texture2D sprite;
}JOGADOR_t;

typedef struct fase
{
    char matriz[MATRIZ_ALTURA][MATRIZ_LARGURA];
    char textoVida[1];
    char textoEnergia[3];
    char textoScore[4];
    char textoEscada[2];
    char textoUltimoMinerio[20];
    char textoNivel[1];
    int nivel;
    int scoreDaFase;
}FASE_t;

typedef struct minerio
{
    int posX;
    int posY;
    char letra;
}MINERIO_t;

typedef struct hud
{

}HUD_t;



//PROT�TIPOS DAS FUN��ES
int menu();     //Retorna a escolha do jogador no menu (jogar, ranking ou sair)
void inicializacao();   //inicializa algumas variaveis do jogo, como carregar imagens e salvar posi��o inicial do jogador
void input();   //recebe os comandos realizados pelo teclado -> constantemente sendo chamada
void update();  //atualiza constantemente, usada apenas para verifica��es do jogo que devem estar sempre sendo verificadas
void renderizar();  //renderiza oq deve ser desenhado -> evita bugs entre varios BeginDrawing()
void desenhar();    //Respons�vel pela parte gr�fica -> constantemente sendo chamada para 'mover' a parte grafica do jogo
void sair();    //descarrega imagens
void leitura_fase();    //cria uma matriz no jogo com base no arquivo txt lido -> usada apenas ao carregar o jogo pela 1� vez
void move_jogador();    //troca a posi��o do jogador -> fazendo com q ele seja desenhado na nova posi��o
char verifica_colisores();  //faz a verifica��o dos colisores ao redor do jogador, permitindo ou nao que ele ande
void coloca_escada();   //fun��o para colocar a escada na posi��o do jogador
void atualiza_matriz_jogador();     //atualiza a matriz criada no jogo qnd o jogador se move (tentar unificar)
void atualiza_matriz();     //atualiza a matriz criada no jogo (tentativa de servir pra td -> jogador, escadas, minera��o)
void salva_matriz_no_arquivo();     //salva a matriz do jogo no arquivo txt, sobrescrevendo o mesmo
void troca_sprites_jogador();     //verifica qual deve ser a sprite do jogador no momento(sozinho, com escada ou com picareta)
void sobe_desce_escada();   //troca a posi��o do jogador em cima da escada -> fazendo com a ele seja desenhado na nova posi��o
void minerar();     //faz a troca de caracteres adequada na matriz do jogo
void minera_bloco();
char verifica_queda();
int fim_de_jogo();
void hud();
void gravidade();
void mudar_fase();

void imprime_matriz();  //fun��o pra testes -> imprime uma matriz qlqr
void imprime_matriz_ptr();  //fun��o pra testes -> imprime uma matriz com ponteiros



//VARI�VEIS GLOBAIS
int mapaCarregado = FALSO;
int opcaoMenu = 0;

/*  COMENT�RIOS - PROBLEMAS - POSSIVEIS SOLU��ES - A FAZERES

    SE DER TEMPO -> ARRUMAR MATRIZ: Qnd jogador sai do jogo em cima de uma escada ele desaparece

*/

int main()
{
    //MENU                        X ,  Y                                X ,  Y                             X ,  Y
    int posJogar[COORDENADAS] = {545, 440}, posRanking[COORDENADAS] = {545, 500}, posSair[COORDENADAS] = {545, 560};
    int posXSetaOpcao = 520;

    //TYPEDEFS
    SPRITE_t sprite;
    JOGADOR_t jogador;
    FASE_t fase;
    MINERIO_t minerio;

    inicializacao(&sprite, &jogador, &fase);

    while (!WindowShouldClose())
    {
        //RECEBENDO A OP��O RETORNADA DO MENU
        if(opcaoMenu == 0)
        {
            opcaoMenu = menu(posJogar, posRanking, posSair, posXSetaOpcao, sprite);
        }

        //MENU RETORNOU A OP��O DE JOGAR
        if(opcaoMenu == posJogar[Y])
        {
            if(mapaCarregado == FALSO)
            {
                //ler a matriz do arquivo texto para carregar o mapa inicial
                leitura_fase(&fase, &jogador);
            }

            input(&jogador, &fase, sprite, &minerio);
            update(jogador, fase, sprite);
            renderizar(sprite, jogador, fase);
        }

        //MENU RETORNOU OP��O DE ABRIR O RANKING
        if(opcaoMenu == posRanking[Y])
        {

        }

        //MENU RETORNOU OP��O DE SAIR
        if(opcaoMenu == posSair[Y])
        {
            sair(sprite);
        }
    }

    return 0;
}


void inicializacao(SPRITE_t *sprite, JOGADOR_t *jogador, FASE_t *fase)
{
    InitWindow(TELA_LARGURA, TELA_ALTURA, "Jogo");
    SetTargetFPS(60);

    sprite->capa = LoadTexture("C:/TerraINF/Imagens/capa.png");
    sprite->jogador = LoadTexture("C:/TerraINF/Imagens/jogador.png");
    sprite->jogador_escada = LoadTexture("C:/TerraINF/Imagens/jogador_escada.png");
    sprite->jogador_picareta = LoadTexture("C:/TerraINF/Imagens/jogador_picareta.png");
    sprite->fundoTerra = LoadTexture("C:/TerraINF/Imagens/fundo_terra.png");
    sprite->borda = LoadTexture("C:/TerraINF/Imagens/borda.png");
    sprite->terra = LoadTexture("C:/TerraINF/Imagens/terra.png");
    sprite->escada = LoadTexture("C:/TerraINF/Imagens/escada.png");
    sprite->minerio = LoadTexture("C:/TerraINF/Imagens/minerio.png");
    sprite->hud = LoadTexture("C:/TerraINF/Imagens/fundo_hud.png");
    sprite->gameOver = LoadTexture("C:/TerraINF/Imagens/gameover.png");

    jogador->sprite = sprite->jogador;
    jogador->emCimaDaEscada = FALSO;
    jogador->modoMineracao = DESATIVADO;    //desligado

    jogador->vida = 3;
    jogador->energia = 100;
    jogador->score = 0;
    jogador->numEscadas = 20;

    fase->nivel = 1;
    fase->scoreDaFase = 1000;

}

void input(JOGADOR_t *jogador, FASE_t *fase, SPRITE_t sprite, MINERIO_t *minerio)
{
    char tecla;
    int proximaPosX = jogador->posX, proximaPosY = jogador->posY;
    int posAbaixoJogador = jogador->posY;
    char letraColisor;

    //verifica��o se jogador apertou shift
    if (IsKeyPressed(KEY_LEFT_SHIFT))
    {
        coloca_escada(fase, jogador, sprite);
    }

    tecla = GetCharPressed();
    switch(tolower(tecla)) //para usar o tolower() precisa usar o include<ctype.h>
    {
        case 'a':
            proximaPosX -=  TILE_TAM;
            if(jogador->modoMineracao == DESATIVADO)
            {
                move_jogador(proximaPosX, jogador->posY, jogador, *fase, sprite);
            }
            else
            {
                letraColisor = verifica_colisores(fase, proximaPosX, jogador->posY, jogador);
                minerio->letra = letraColisor;
                minerio->posX = proximaPosX;
                minerio->posY = jogador->posY;
            }
            break;
        case 'd':
            proximaPosX +=  TILE_TAM;

            if(jogador->modoMineracao == DESATIVADO)
            {
                move_jogador(proximaPosX, jogador->posY, jogador, *fase, sprite);
            }
            else
            {
                letraColisor = verifica_colisores(fase, proximaPosX, jogador->posY, jogador);
                minerio->letra = letraColisor;
                minerio->posX = proximaPosX;
                minerio->posY = jogador->posY;
            }
            break;
        case 'w':
            proximaPosY -=  TILE_TAM; //fazer logica pra funcionar so qnd tiver uma escada
            if(jogador->modoMineracao == DESATIVADO)
            {
                sobe_desce_escada(jogador->posX, proximaPosY, jogador, *fase, sprite);
            }
            else
            {
                letraColisor = verifica_colisores(fase, jogador->posX, proximaPosY, jogador);
                minerio->letra = letraColisor;
                minerio->posX = jogador->posX;
                minerio->posY = proximaPosY;
            }
            break;
        case 's':
            proximaPosY +=  TILE_TAM; //fazer logica pra funcionar so qnd tiver uma escada
            if(jogador->modoMineracao == DESATIVADO)
            {
                sobe_desce_escada(jogador->posX, proximaPosY, jogador, *fase, sprite);
            }
            else
            {
                letraColisor = verifica_colisores(fase, jogador->posX, proximaPosY, jogador);
                minerio->letra = letraColisor;
                minerio->posX = jogador->posX;
                minerio->posY = proximaPosY;
            }
            break;
        case 49:
            //apertei 1 -> modo minera�ao ativado
            if(jogador->modoMineracao == DESATIVADO)
            {
                jogador->modoMineracao = ATIVADO;
                troca_sprites_jogador(jogador, sprite.jogador_picareta);
            }
            else
            {
                jogador->modoMineracao = DESATIVADO;
                troca_sprites_jogador(jogador, sprite.jogador);
            }

            break;
        case ' ':
            minera_bloco(fase, *minerio, jogador, sprite);
            break;
    }
}

void minera_bloco(FASE_t *fase, MINERIO_t minerio, JOGADOR_t *jogador, SPRITE_t sprite)
{
    int posMinerioX, posMinerioY;

    posMinerioX = minerio.posX/TILE_TAM;
    posMinerioY = minerio.posY/TILE_TAM;

    switch(tolower(minerio.letra))
    {
        case 'x':
            jogador->energia -= 3;
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        case 'c':
            jogador->energia -= 20;
            strcpy(fase->textoUltimoMinerio, "Cesio");
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        case 'g':
            jogador->score += 100;
            jogador->energia += 20;

            //strcpy(fase->textoUltimoMinerio, "Ouro");
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        case 't':
            jogador->score += 150;
            jogador->energia += 30;
            //strcpy(fase->textoUltimoMinerio, "Titanium");
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        case 's':
            jogador->score += 50;
            jogador->energia += 10;
            //strcpy(fase->textoUltimoMinerio, "Prata");
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        case 'u':
            jogador->energia -= 30;
            //strcpy(fase->textoUltimoMinerio, "Uranio");
            fase->matriz[posMinerioY][posMinerioX] = ' ';
            break;
        default:
            //printf("\n\nNAO PODE MINERAR ESSE BLOCO\n\n");
            break;
    }

    if(jogador->score >= fase->scoreDaFase)
    {
        mudar_fase(fase, jogador);

    }

    gravidade(jogador, *fase, sprite);

    if(jogador->energia <= 20)
    {
        jogador->vida--;
        jogador->energia = 100;
    }

    //atualiza_matriz(posMinerioX, posMinerioY, fase, &jogador);
    imprime_matriz_ptr(fase);
}

void mudar_fase(FASE_t *fase, JOGADOR_t *jogador)
{
    fase->nivel++;
    fase->scoreDaFase *= 2;
    jogador->score = 0;
    jogador->numEscadas = 20;
    jogador->vida = 3;
    jogador->energia = 100;
    jogador->modoMineracao = DESATIVADO;
    jogador->emCimaDaEscada = FALSO;

    leitura_fase(fase, jogador);
}

void sobe_desce_escada(int proxPosX, int proxPosY, JOGADOR_t *jogador, FASE_t fase, SPRITE_t sprite)
{
    char letraColisor;

    letraColisor = verifica_colisores(&fase, proxPosX, proxPosY, jogador);
    if(letraColisor == 'H')
    {
        jogador->posY = proxPosY;
        troca_sprites_jogador(jogador, sprite.jogador_escada);
    }
    else if(jogador->emCimaDaEscada == VDD)
    {
        jogador->posY = proxPosY;
        troca_sprites_jogador(jogador, sprite.jogador);
    }
    atualiza_matriz(jogador->posX, jogador->posY, &fase, jogador);
}

char verifica_colisores(FASE_t *fase, int posX, int posY, JOGADOR_t *jogador)
{
    posX = posX/TILE_TAM;
    posY = posY/TILE_TAM;

    //printf("\n\nCHEGOU NO COLISOR: %c \n\n", fase->matriz[posY][posX]);

    if(fase->matriz[posY + 1][posX] == 'H')
    {
        jogador->emCimaDaEscada = VDD;
    }
    else
    {
        jogador->emCimaDaEscada = FALSO;
    }

    return fase->matriz[posY][posX];
}

char verifica_queda(FASE_t *fase, int proximaPosX, int posAbaixoJogador, JOGADOR_t *jogador)
{
    proximaPosX = proximaPosX/TILE_TAM;
    posAbaixoJogador = posAbaixoJogador/TILE_TAM;

    return fase->matriz[posAbaixoJogador][proximaPosX];
}

void gravidade(JOGADOR_t *jogador, FASE_t fase, SPRITE_t sprite)
{
    char letraQueda;
    int posAbaixoJogador = jogador->posY;
    int altura = 0;

    letraQueda = verifica_queda(&fase, jogador->posX, posAbaixoJogador, jogador);

    while(letraQueda == ' ')
    {
        jogador->posY = posAbaixoJogador;
        posAbaixoJogador = jogador->posY + TILE_TAM;
        letraQueda = verifica_queda(&fase, jogador->posX, posAbaixoJogador, jogador);
        altura++;
    }

    if(altura > 3)
    {
        jogador->vida --;
        if(jogador->vida == 0)
        {
            printf("\n\nVidas: %d \n\n", jogador->vida);
            opcaoMenu = fim_de_jogo(sprite);
        }
    }
}

void move_jogador(int proximaPosX, int posAtualJogadorY, JOGADOR_t *jogador, FASE_t fase, SPRITE_t sprite)
{
    char letraColisor;

    letraColisor = verifica_colisores(&fase, proximaPosX, posAtualJogadorY, jogador);

    if(letraColisor == ' ')
    {
        jogador->posX = proximaPosX;
        troca_sprites_jogador(jogador, sprite.jogador);
    }

    if(letraColisor == 'H')
    {
        jogador->posX = proximaPosX;
        troca_sprites_jogador(jogador, sprite.jogador_escada);
    }

    gravidade(jogador, fase, sprite);

    atualiza_matriz(jogador->posX, jogador->posY, &fase, jogador);
}

void atualiza_matriz(int posX, int posY, FASE_t *fase, JOGADOR_t *jogador)
{
    posX = posX/TILE_TAM;
    posY = posY/TILE_TAM;

    //printf("\n\nELEMENTO: %c \n\n", fase->matriz[posY][posX]);

    fase->matriz[posY][posX] = 'J';

    imprime_matriz_ptr(fase);
    //salva_matriz_no_arquivo(*fase);

}


void salva_matriz_no_arquivo(FASE_t fase)
{
    int j;
    FILE *arqFase;

    if(fase.nivel == 1)
    {
        arqFase = fopen("nivel1.txt", "w");
    }
    if(fase.nivel == 2)
    {
        arqFase = fopen("nivel2.txt", "w");
    }

    if(arqFase == NULL)
    {
        printf("Erro ao abrir o arquivo");
    }

    for(int i = 0; i < MATRIZ_ALTURA; i++)
    {
        for(j = 0; j < MATRIZ_LARGURA; j++)
        {
            fprintf(arqFase, "%c", fase.matriz[i][j]);
        }
        fprintf(arqFase, "\n");
    }

    fclose(arqFase);

}

void coloca_escada(FASE_t *fase, JOGADOR_t *jogador, SPRITE_t sprite)
{
    int posX, posY;

    posX = jogador->posX/TILE_TAM;
    posY = jogador->posY/TILE_TAM;

    if(jogador->emCimaDaEscada == FALSO)
    {
        jogador->emCimaDaEscada = VDD;
    }

    if(jogador->numEscadas > 0)
    {
        fase->matriz[posY][posX] = 'H';
        troca_sprites_jogador(jogador, sprite.jogador_escada);
        jogador->numEscadas--;
    }
}

void update(JOGADOR_t jogador, FASE_t fase, SPRITE_t sprite)
{
    SetExitKey(KEY_L); //atalho pra sair do jogo -> REMOVER DEPOIS
}

//Fun��o respons�vel pela renderiza��o do jogo (usada principalmente pra n ter mais de um BeginDrawing)
void renderizar(SPRITE_t sprite, JOGADOR_t jogador, FASE_t fase)
{
    BeginDrawing();
    ClearBackground(WHITE);
    desenhar(sprite, &jogador, fase);
    hud(&fase, &jogador);
    EndDrawing();
}

void hud(FASE_t *fase, JOGADOR_t *jogador)
{
    sprintf(fase->textoScore,"%d", jogador->score);
    DrawText("SCORE: ", 5, 10, 20, WHITE);
    DrawText(fase->textoScore, 90, 10, 20, GREEN);

    sprintf(fase->textoVida,"%d", jogador->vida);
    DrawText("VIDAS: ", 250, 10, 20, WHITE);
    DrawText(fase->textoVida, 330, 10, 20, GREEN);

    sprintf(fase->textoEnergia,"%d", jogador->energia);
    DrawText("ENERGIA: ", 400, 10, 20, WHITE);
    DrawText(fase->textoEnergia, 500, 10, 20, GREEN);

    sprintf(fase->textoEscada,"%d", jogador->numEscadas);
    DrawText("ESCADAS: ", 600, 10, 20, WHITE);
    DrawText(fase->textoEscada, 710, 10, 20, GREEN);

    sprintf(fase->textoNivel,"%d", fase->nivel);
    DrawText("FASE: ", 790, 10, 20, WHITE);
    DrawText(fase->textoNivel, 850, 10, 20, GREEN);

    DrawText("ITEM: ", 890, 10, 20, WHITE);
    DrawText(fase->textoUltimoMinerio, 960, 10, 20, WHITE);



}

//Fun��o respons�vel por de fato desenhar oq foi renderizado
void desenhar(SPRITE_t sprite, JOGADOR_t *jogador, FASE_t fase)
{
    for(int i = 0; i < MATRIZ_ALTURA; i++)
    {
        for(int j = 0; j < MATRIZ_LARGURA; j++)
        {
            switch(fase.matriz[i][j]) //nao sei pq inverte o i com o j, mas ta funcionando
            {
                case 'J': DrawTexture(sprite.fundoTerra, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'G': DrawTexture(sprite.minerio, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'T': DrawTexture(sprite.minerio, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'C': DrawTexture(sprite.minerio, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'S': DrawTexture(sprite.minerio, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'U': DrawTexture(sprite.minerio, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'B': DrawTexture(sprite.borda, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'X': DrawTexture(sprite.terra, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case 'H': DrawTexture(sprite.escada, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
                case ' ': DrawTexture(sprite.fundoTerra, j * TILE_TAM, i * TILE_TAM , WHITE);
                    break;
            }
            if(i <= 30 && j == 0)
            {
                DrawTexture(sprite.hud, i * TILE_TAM, j * TILE_TAM , WHITE);
            }
        }
    }
    DrawTexture(jogador->sprite, jogador->posX, jogador->posY, WHITE);
}

void leitura_fase(FASE_t *fase, JOGADOR_t *jogador)
{
    FILE *arqFase;
    int i, j;

    if(fase->nivel == 1)
    {
         arqFase = fopen("nivel1.txt", "r");
    }
    if(fase->nivel == 2)
    {
         arqFase = fopen("nivel2.txt", "r");
    }

    if(arqFase == NULL)
    {
        printf("\n\nErro ao abrir o arquivo\n\n");
    }

    for(i = 0; i < MATRIZ_ALTURA; i++)
    {
        for(j = 0; j < MATRIZ_LARGURA; j++)
        {
            fscanf(arqFase, "%c", &fase->matriz[i][j]);

            if(fase->matriz[i][j] == 'J')
            {
                jogador->posX = j * TILE_TAM;
                jogador->posY = i * TILE_TAM;
                fase->matriz[i][j] = ' ';
            }
            else if(fase->matriz[i][j] == 'K')
            {
                jogador->posX = j * TILE_TAM;
                jogador->posY = i * TILE_TAM;
                fase->matriz[i][j] = 'H';
            }
        }
        // le o caractere de nova linha(\n) para avan�ar para a pr�xima linha do arquivo
        fscanf(arqFase, "%c\n", &fase->matriz[i][j]);
    }
    fclose(arqFase);

    mapaCarregado = VDD;
}


//Fun��o respons�vel pela logica do menu -> basicamente de acordo com a posi��o Y de cada opcao, � poss�vel selecionar a op��o desejada
int menu(int posJogar[2], int posRanking[2], int posSair[2], int posXSetaOpcao, SPRITE_t sprite)
{
    int difEntreLinhas = (posRanking[Y] - posJogar[Y]);
    int opcao = posJogar[Y];
    int selConfirmada = FALSO;
    char teclaMenu;

    char jogar[] = {"Jogar"}, ranking[] = {"Ranking"}, sair[] = {"Sair"}, selOpcao[] = {">"};

    while(selConfirmada == FALSO)
    {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(sprite.capa, 0, 0, WHITE);
        //TEXTOS
        DrawText(jogar, posJogar[X], posJogar[Y], 30, LIGHTGRAY);
        DrawText(ranking, posRanking[X], posRanking[Y], 30, LIGHTGRAY);
        DrawText(sair, posSair[X], posSair[Y], 30, LIGHTGRAY);
        DrawText("Navegar: w/s\nSelecao: espaco", 1000, 445, 20, LIGHTGRAY);

        teclaMenu = GetCharPressed();
        switch(tolower(teclaMenu))
        {
            case 's':
                if(opcao == posSair[Y])
                    opcao = posJogar[Y];
                else
                    opcao += difEntreLinhas;
                break;
            case 'w':
                if(opcao == posJogar[Y])
                    opcao = posSair[Y];
                else
                    opcao -= difEntreLinhas;
                break;
            case ' ': selConfirmada = VDD;
                break;
        }

        DrawText(selOpcao, posXSetaOpcao, opcao, 30, LIGHTGRAY);
        EndDrawing();
    }
    return opcao;
}
int fim_de_jogo(SPRITE_t sprite)
{
    while(GetCharPressed() != ' ')
    {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(sprite.gameOver, 0, 0, WHITE);
        DrawText("GAME OVER", 425, 260, 50, LIGHTGRAY);
        DrawText("Pressione espaco para sair", 360, 360, 30, LIGHTGRAY);
        EndDrawing();
    }

    return 0;
}

//Fun��o respons�vel por descarregar as imagens e fechar a janela do jogo -> Contr�ria � inicializa��o
void sair(SPRITE_t sprite)
{
    UnloadTexture(sprite.terra);
    UnloadTexture(sprite.minerio);
    UnloadTexture(sprite.hud);
    UnloadTexture(sprite.jogador);
    UnloadTexture(sprite.jogador_escada);
    UnloadTexture(sprite.jogador_picareta);
    UnloadTexture(sprite.fundoTerra);
    UnloadTexture(sprite.borda);
    UnloadTexture(sprite.escada);
    UnloadTexture(sprite.capa);
    UnloadTexture(sprite.gameOver);

    CloseWindow();
}

void troca_sprites_jogador(JOGADOR_t *jogador, Texture2D sprite_a_receber)
{
    jogador->sprite = sprite_a_receber;
}




void imprime_matriz_ptr(FASE_t *fase)
{
    for (int i = 0; i < MATRIZ_ALTURA; i++)
    {
      for (int j = 0; j < MATRIZ_LARGURA; j++)
      {
          printf("%c", fase->matriz[i][j]);
      }
      printf("\n");
    }
}

void imprime_matriz(FASE_t fase)
{
    for (int i = 0; i < MATRIZ_ALTURA; i++)
    {
      for (int j = 0; j < MATRIZ_LARGURA; j++)
      {
          printf("%c", fase.matriz[i][j]);
      }
      printf("\n");
    }
}

















