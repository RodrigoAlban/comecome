#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "comecome.h"
#include "mapa.h"

MAPA m;
POSICAO heroi;
POSICAO initial_heroi;
int tempilula = 0;
int score = 0;
int lives = 3;
int pills_left;
int highscore = 0;

int praondefantasmavai (int xatual, int yatual, int* xdestino, int* ydestino){

    int opcoes[4][2] = {
        {xatual, yatual+1},
        {xatual+1, yatual},
        {xatual, yatual-1},
        {xatual-1, yatual}
    };

    int best_dist = 999;
    int best_i = -1;
    for(int i = 0; i < 4; i++){
        if(podeandar(&m, FANTASMA, opcoes[i][0], opcoes[i][1])){
            int dist = abs(opcoes[i][0] - heroi.x) + abs(opcoes[i][1] - heroi.y);
            if(dist < best_dist){
                best_dist = dist;
                best_i = i;
            }
        }
    }
    if(best_i != -1){
        *xdestino = opcoes[best_i][0];
        *ydestino = opcoes[best_i][1];
        return 1;
    }
    return 0;
}

void fantasmas(){
    MAPA copia;

    copiamapa(&copia, &m);

    for(int i = 0; i < m.linhas; i++){
        for(int j = 0; j < m.colunas; j++){
            
            if(copia.matriz[i][j] == FANTASMA) {

                int xdestino;
                int ydestino;

                int encontrou=praondefantasmavai(i, j, &xdestino, &ydestino);

                    if(encontrou) {
                        andanomapa(&m, i, j, xdestino, ydestino);
                    }
                }
            }
        }
    liberamapa(&copia);
}

int ehdirecao(char direcao) {
    return direcao == 'a' ||
           direcao == 'w' ||
           direcao == 's' ||
           direcao == 'd';
}

void move(char direcao){

    if(!ehdirecao(direcao))
        return;

    int proximox = heroi.x;
    int proximoy = heroi.y;

    switch(direcao){
        case ESQUERDA: 
            proximoy--;
            break;
        case CIMA:
            proximox--;
            break;
        case BAIXO:
            proximox++;
            break;
        case DIREITA:
            proximoy++;
            break;
    }
    if(!podeandar(&m, HEROI, proximox, proximoy))
        return;
    
    if(ehpersonagem(&m, FANTASMA, proximox, proximoy)){
        if(!tempilula){
            lives--;
            if(lives > 0){
                // Limpa a posição anterior do herói
                m.matriz[heroi.x][heroi.y] = VAZIO;
                heroi = initial_heroi;
                m.matriz[heroi.x][heroi.y] = HEROI;
            }
            return;
        } else {
            score += 100;
        }
    }

    if(ehpersonagem(&m, PILULA, proximox, proximoy)){
        tempilula = 1;
        m.matriz[proximox][proximoy] = VAZIO;
        score += 10;
        pills_left--;
    }
    if(ehpersonagem(&m, CHERRY, proximox, proximoy)){
        m.matriz[proximox][proximoy] = VAZIO;
        score += 50;
    }
    
    andanomapa(&m, heroi.x, heroi.y, proximox, proximoy);
    heroi.x = proximox;
    heroi.y = proximoy;
}

int acabou() {
    POSICAO pos;

    int perdeu = (lives <= 0) || !encontramapa(&m, &pos, HEROI);
    int ganhou = !encontramapa(&m, &pos, FANTASMA);

    return ganhou || perdeu;

}

void explodepilula(){
    if(!tempilula) return;

    explodepilula2(heroi.x, heroi.y, 0, 1, 3);
    explodepilula2(heroi.x, heroi.y, 0, -1, 3);
    explodepilula2(heroi.x, heroi.y, 1, 0, 3);
    explodepilula2(heroi.x, heroi.y, -1, 0, 3);

    tempilula = 0;

}
void explodepilula2(int x, int y, int somax, int somay, int qtd) {
    if(qtd == 0) return;
    
    int novox = x + somax;
    int novoy = y + somay;

    if(!ehvalida(&m, novox, novoy)) return;
    if(ehparede(&m, novox, novoy)) return;

    if(m.matriz[novox][novoy] == FANTASMA) score += 100;
    m.matriz[novox][novoy] = VAZIO;
    explodepilula2(novox, novoy, somax, somay, qtd-1);
}

int main() {

    srand(time(NULL));
    FILE* f = fopen("highscore.txt", "r");
    if(f){
        fscanf(f, "%d", &highscore);
        fclose(f);
    }
    lemapa(&m);
    encontramapa(&m, &heroi, HEROI);
    initial_heroi = heroi;
    pills_left = contapersonagem(&m, PILULA);

    printf("Bem-vindo ao ComeCome!\n");
    printf("Use w,a,s,d para mover, b para bomba quando tiver a pílula.\n");
    printf("Colete todas as pílulas ou elimine todos os fantasmas para vencer!\n");

    POSICAO pos;
    do{
        printf("Pontuação: %d (Recorde: %d) Vidas: %d Pílulas restantes: %d Tem pílula: %s\n", score, highscore, lives, pills_left, (tempilula ? "SIM" : "NÃO"));
        imprimemapa(&m);

        char comando;
        scanf(" %c", &comando);
        move(comando);
        if(comando == BOMBA) explodepilula();
        fantasmas();

    } while(!acabou());

    int perdeu = (lives <= 0);
    int ganhou = !encontramapa(&m, &pos, FANTASMA);

    if(perdeu){
        printf("Fim de Jogo! Pontuação Final: %d\n", score);
    } else if(ganhou){
        printf("Você Venceu! Pontuação: %d\n", score);
    }

    if(score > highscore){
        highscore = score;
        FILE* f = fopen("highscore.txt", "w");
        if(f){
            fprintf(f, "%d", highscore);
            fclose(f);
        }
    }

    liberamapa(&m);

}