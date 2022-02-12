#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// ----------------------------- CONSTANTES E TIPOS ----------------------------------------
const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;


const int VELOCIDADE_NAVE = 6;
const int TAMANHO_ALTURA_NAVE = 90;
const int TAMANHO_LARGURA_NAVE = 108;

const int RAIO_MAXIMO_TIRO = 15;
const int TAMANHO_MINIMO_TIRO = 7;
const int VELOCIDADE_TIRO = 13;

const int VELOCIDADE_BLOCO = 2;

const int QUANTIDADE_INIMIGOS = 20;
const int VELOCIDADE_MAXIMA_INIMIGO = 10;
const int RAIO_MAXIMO_INIMIGO = 50;
const int RAIO_MINIMO_INIMIGO = 15;

ALLEGRO_COLOR COR_CENARIO;

typedef struct Circulo {
	int xCentro, yCentro;
	float raio;
} Circulo;

typedef struct Retangulo {
	int x, y;
	int largura, altura;
} Retangulo;

typedef struct Tiro {
	int xCentro, yCentro;
	float raio;
	int disparado;
	int carregando;
	ALLEGRO_COLOR cor;
} Tiro;

typedef struct Nave {
	int x, y;
	int dirX, dirY;
	int velocidade;
	int largura, altura;
	ALLEGRO_BITMAP *imagem;
	Circulo hitbox;
	Tiro tiro;
	int pontuacao;
} Nave;

typedef struct Bloco {
	int x,y;
	int largura, altura;
	ALLEGRO_COLOR cor;
} Bloco;

typedef struct Inimigo {
	int id;
	int xCentro, yCentro;
	int velocidade;
	float raio;
	ALLEGRO_COLOR cor;
} Inimigo;



void initGlobais(){
	COR_CENARIO = al_map_rgb(41,33,50);
}







// ------------------------- funcoes de inicializacao ----------------------------






void atualizaHitboxNave(Nave * nave){
	nave->hitbox.raio = nave->largura < nave->altura ? (nave->largura / 2) : (nave->altura / 2);
	nave->hitbox.xCentro = nave->x + (nave->largura / 2);
	nave->hitbox.yCentro = nave->y + (nave->altura / 2);
}

void initTiro(Nave * nave){
	nave->tiro.xCentro = nave->x + nave->largura;
	nave->tiro.yCentro = nave->y + (nave->altura / 2);
	nave->tiro.raio = 0;
	nave->tiro.disparado = 0;
	nave->tiro.carregando = 0;

	nave->tiro.cor = al_map_rgb(255,0,0);
}


void initNave(Nave * nave){
	nave->largura = TAMANHO_LARGURA_NAVE;
	nave->altura = TAMANHO_ALTURA_NAVE;

	nave->x = 10 + nave->largura;
	nave->y = SCREEN_H/2;

	nave->dirX = 0;
	nave->dirY = 0;
	nave->velocidade = VELOCIDADE_NAVE;

	nave->imagem = al_load_bitmap("swordfish.png");

	nave->pontuacao = 0;

	atualizaHitboxNave(nave);
	initTiro(nave);
}


void initBloco(Bloco * bloco){
	bloco->x = ( 2.5 * SCREEN_W ) + (rand() % SCREEN_W); 
	bloco->y = rand() % ( 4 * SCREEN_H /5);
	bloco->largura =  (1.5 * SCREEN_W) + rand() % (SCREEN_W); 
	bloco->altura = SCREEN_H/5 +  rand() % (2 * SCREEN_H/5);
	bloco->cor = al_map_rgb( 255,255,255);
}

void initInimigo(Inimigo * inimigo,  int id){
	inimigo->id = id;

	inimigo->raio = (rand() % RAIO_MAXIMO_INIMIGO) + RAIO_MINIMO_INIMIGO; 

	int diametro = inimigo->raio * 2;
	inimigo->xCentro = ( 1.3 * SCREEN_W ) + rand() % (SCREEN_W); 
	inimigo->yCentro = inimigo->raio +  (rand() % (SCREEN_H - diametro));

	inimigo->cor = al_map_rgb(rand(),rand(),rand());

	inimigo->velocidade = rand() % VELOCIDADE_MAXIMA_INIMIGO + 1;
}

void initInimigos(Inimigo inimigos[QUANTIDADE_INIMIGOS]){
	int i;
	for(i=0; i< QUANTIDADE_INIMIGOS; i++){
		initInimigo(&inimigos[i], i);
	}
}




// --------------------------- funcoes de destruicao -------------------






void destroiTiro(Nave * nave){
	initTiro(nave);
}

void destroiInimigo(Inimigo * inimigo){
	initInimigo(inimigo, inimigo->id);
}







// --------------------------- Funcoes de Desenho -------------------------







void desenhaCenario() {
	al_clear_to_color(COR_CENARIO);
}

void desenhaTiro(Tiro tiro){
	al_draw_filled_circle(tiro.xCentro, tiro.yCentro, tiro.raio, tiro.cor );
}

void desenhaNave(Nave nave){
	al_draw_scaled_bitmap(nave.imagem,
		0, 0,
	 	al_get_bitmap_width(nave.imagem),al_get_bitmap_height(nave.imagem),
		nave.x, nave.y,
		nave.largura,nave.altura,
		0
	);

	desenhaTiro(nave.tiro);
}

void desenhaBloco(Bloco bloco){
	al_draw_filled_rectangle(bloco.x, bloco.y, bloco.x + bloco.largura, bloco.y + bloco.altura, bloco.cor );
}

void desenhaInimigo(Inimigo inimigo){
	al_draw_filled_circle(inimigo.xCentro, inimigo.yCentro, inimigo.raio, inimigo.cor );
}

void desenhaInimigos(Inimigo inimigos[QUANTIDADE_INIMIGOS]){
	int i;
	for(i=0; i< QUANTIDADE_INIMIGOS; i++){
		desenhaInimigo(inimigos[i]);
	}
}





// --------------------- Funções de Atualizacao -----------------------------






void atualizaTiro(Nave * nave){
	if(nave->tiro.carregando && nave->tiro.raio <= RAIO_MAXIMO_TIRO ){
		if(nave->tiro.raio == 0){
			nave->tiro.raio += TAMANHO_MINIMO_TIRO;
		}
		nave->tiro.raio += 0.1;
	} 

	if(nave->tiro.disparado){
		nave->tiro.xCentro += VELOCIDADE_TIRO;
	} else {
		nave->tiro.xCentro = nave->x + nave->largura;
		nave->tiro.yCentro = nave->y + (nave->altura / 2);
	}
}

void atualizaNave(Nave * nave){
	
	if( nave->x <= 0 && nave->dirX < 0){
		nave->x = 0;
	} else if (nave->x + nave->largura >= SCREEN_W && nave->dirX > 0 ){
		nave->x = SCREEN_W - nave->largura;
	} else {
		nave->x += (nave->dirX * nave->velocidade);
	}

	if( nave->y <= 0  &&  nave->dirY < 0 ){
		nave->y = 0;
	} else if(nave->y + nave->altura >= SCREEN_H  && nave->dirY > 0) { 
		nave->y = SCREEN_H - nave->altura;
	} else {
		nave->y += (nave->dirY * nave->velocidade);
	}

	atualizaHitboxNave(nave);
	atualizaTiro(nave);
		
}

void atualizaBloco(Bloco * bloco){
	bloco->x -= VELOCIDADE_BLOCO;

	if( (bloco->x + bloco->largura)  < 0 ){
		initBloco(bloco);
	}
}

void atualizaInimigo(Inimigo * inimigo){
	if(inimigo->xCentro + inimigo->raio <= 0){
		destroiInimigo(inimigo);
	}
	inimigo->xCentro -= inimigo->velocidade;

}

void atualizaInimigos(Inimigo inimigos[QUANTIDADE_INIMIGOS]){
	int i;
	for(i=0; i< QUANTIDADE_INIMIGOS; i++){
		atualizaInimigo(&inimigos[i]);
	}
}






// ----------------------------- funcoes de colisão -------------------------







Retangulo criaRetangulo(int x, int y, int largura, int altura){
	Retangulo retangulo;
	retangulo.x = x;
	retangulo.y = y;
	retangulo.largura = largura;
	retangulo.altura = altura;

	return retangulo;
}

Circulo criaCirculo(int xCentro, int yCentro, float raio){
	Circulo circulo;
	circulo.xCentro = xCentro;
	circulo.yCentro = yCentro;
	circulo.raio = raio;

	return circulo;
}


float calculaDistanciaPontos(int xPrimeiro, int yPrimeiro, int xSegundo, int ySegundo){
	int distanciaX = xPrimeiro - xSegundo;
	int distanciaY = yPrimeiro - ySegundo;

	return sqrt( pow(distanciaX,2) + pow(distanciaY,2));
}

int validaColisaoCirculoRetangulo(Circulo circulo, Retangulo retangulo){
	// valida colisao acima e abaixo
	if(circulo.xCentro >= retangulo.x && circulo.xCentro <= retangulo.x + retangulo.largura){
		if( circulo.yCentro + circulo.raio >= retangulo.y && 
			circulo.yCentro - circulo.raio <= retangulo.y + retangulo.altura ){
			return 1;
			
		}
	}

	// valida colisao direita e esquerda
	if(circulo.yCentro >= retangulo.y && circulo.yCentro <= retangulo.y + retangulo.altura){
		if(circulo.xCentro + circulo.raio >= retangulo.x &&
			circulo.xCentro - circulo.raio <= retangulo.x + retangulo.largura ){
			return 1;
		}
	}
	
	
	// valida colisao canto superior esquerdo
	if( calculaDistanciaPontos(circulo.xCentro, circulo.yCentro, retangulo.x, retangulo.y) <= circulo.raio){
		return 1;
	}

	// valida colisao canto superior direito
	if( calculaDistanciaPontos(circulo.xCentro, circulo.yCentro, retangulo.x + retangulo.largura, retangulo.y) <= circulo.raio){
		return 1;
	}

	// valida colisao canto inferior esquerdo
	if( calculaDistanciaPontos(circulo.xCentro, circulo.yCentro, retangulo.x, retangulo.y + retangulo.altura) <= circulo.raio){
		return 1;
	}

	// valida colisao canto inferior direito
	if( calculaDistanciaPontos(circulo.xCentro, circulo.yCentro, retangulo.x + retangulo.largura, retangulo.y + retangulo.altura) <= circulo.raio){
		return 1;
	}

	
	return 0;

}

int validaColisaoCirculos(Circulo primeiro, Circulo segundo){
	 return calculaDistanciaPontos(primeiro.xCentro, primeiro.yCentro, segundo.xCentro, segundo.yCentro) <= primeiro.raio + segundo.raio;
}

float validaColisaoCirculoInimigos(Circulo circulo, Inimigo inimigos[]){
		Circulo hitboxInimigo;
		int i, raio;
		for(i=0; i < QUANTIDADE_INIMIGOS; i++ ){
			hitboxInimigo = criaCirculo(inimigos[i].xCentro, inimigos[i].yCentro, inimigos[i].raio);
			if( validaColisaoCirculos(circulo, hitboxInimigo)){
				destroiInimigo(&inimigos[i]);
				return hitboxInimigo.raio;
			}
		}

		return 0;
}

int validaColisaoRetanguloInimigos(Retangulo retangulo, Inimigo inimigos[]){
		Circulo hitboxInimigo;
		int i;
		for(i=0; i < QUANTIDADE_INIMIGOS; i++ ){
			hitboxInimigo = criaCirculo(inimigos[i].xCentro, inimigos[i].yCentro, inimigos[i].raio);
			if( validaColisaoCirculoRetangulo(hitboxInimigo, retangulo)){
				destroiInimigo(&inimigos[i]);
				return 1;
			}
		}

		return 0;
}



int  validaColisaoNave(Nave nave, Bloco bloco, Inimigo inimigos[]){
	Retangulo hitboxBloco;
	hitboxBloco = criaRetangulo( bloco.x, bloco.y, bloco.largura, bloco.altura);
	
	return validaColisaoCirculoRetangulo(nave.hitbox, hitboxBloco) || validaColisaoCirculoInimigos(nave.hitbox, inimigos);

}

void validaColisaoTiro(Nave * nave, Bloco bloco, Inimigo inimigos[]){
	if(!nave->tiro.disparado){
		return;
	}

	// valida colisao tiro x Tela
	if(nave->tiro.xCentro > SCREEN_W){
		destroiTiro(nave);
	}

  	Retangulo hitboxBloco;
	hitboxBloco = criaRetangulo( bloco.x, bloco.y, bloco.largura, bloco.altura);

	Circulo hitboxTiro;
	hitboxTiro = criaCirculo(nave->tiro.xCentro, nave->tiro.yCentro, nave->tiro.raio);

	// valida colisao tiro x Bloco
	if(validaColisaoCirculoRetangulo(hitboxTiro, hitboxBloco )){
		destroiTiro(nave);
	}

	int raioInimigoColisao = (int) validaColisaoCirculoInimigos(hitboxTiro, inimigos);
	if(raioInimigoColisao ){
		nave->pontuacao += (raioInimigoColisao / 5);
		if( nave->tiro.raio < RAIO_MAXIMO_TIRO ){
			destroiTiro(nave);
		}
	}

}

int validaColisaoInimigo(Inimigo primeiro , Inimigo segundo){
	Circulo hitboxprimeiro, hitboxSegundo;

	if(primeiro.id == segundo.id){
		return 0;
	}

	hitboxprimeiro = criaCirculo(primeiro.xCentro, primeiro.yCentro, primeiro.raio);
	hitboxSegundo = criaCirculo(segundo.xCentro, segundo.yCentro, segundo.raio);

	return validaColisaoCirculos(hitboxprimeiro, hitboxSegundo);
}

void validaColisaoEntreInimigos(Inimigo inimigos[]){
	Circulo hitboxInimigo;
	int i, j;
	
	for(i=0; i < QUANTIDADE_INIMIGOS; i++ ){
		for(j=0; j < QUANTIDADE_INIMIGOS; j++ ){
			if(validaColisaoInimigo(inimigos[i], inimigos[j])){
				destroiInimigo(&inimigos[i]);
				destroiInimigo(&inimigos[j]);
			}
		}
	}
}

void validaColisaoInimigos(Inimigo inimigos[], Bloco bloco){
	Retangulo hitboxBloco;
	hitboxBloco = criaRetangulo( bloco.x, bloco.y, bloco.largura, bloco.altura);

	validaColisaoRetanguloInimigos(hitboxBloco, inimigos);

	validaColisaoEntreInimigos(inimigos);

}





// --------------------------- funções de evento ------------------------







void manipulaEventoMovimentacaoNave( int tipoEvento, int teclaEvento , Nave * nave ){
	if(tipoEvento == ALLEGRO_EVENT_KEY_DOWN){
		switch(teclaEvento){
			case ALLEGRO_KEY_W:
				nave->dirY--;
			break;
			case ALLEGRO_KEY_S:
				nave->dirY++;
			break;
			case ALLEGRO_KEY_A:
				nave->dirX--;
			break;
			case ALLEGRO_KEY_D:
				nave->dirX++;
			break;
		}
	} else if(tipoEvento == ALLEGRO_EVENT_KEY_UP) {
		switch(teclaEvento){
			case ALLEGRO_KEY_W:
				nave->dirY++;
			break;
			case ALLEGRO_KEY_S:
				nave->dirY--;
			break;
			case ALLEGRO_KEY_A:
				nave->dirX++;
			break;
			case ALLEGRO_KEY_D:
				nave->dirX--;
			break;
		}
	}
}
	

void manipulaEventoTiro(int tipoEvento, int teclaEvento,  Nave * nave){
	if(tipoEvento == ALLEGRO_EVENT_KEY_DOWN &&  teclaEvento == ALLEGRO_KEY_SPACE && nave->tiro.disparado == 0 ){
		nave->tiro.carregando = 1;
	} else if(tipoEvento == ALLEGRO_EVENT_KEY_UP &&  teclaEvento == ALLEGRO_KEY_SPACE && nave->tiro.carregando == 1) {
		nave->tiro.carregando = 0;
		nave->tiro.disparado = 1;
	}
}

void manipulaEventoTeclado( int tipoEvento, int teclaEvento,  Nave * nave ){
	manipulaEventoMovimentacaoNave(tipoEvento, teclaEvento, nave);
	manipulaEventoTiro(tipoEvento, teclaEvento, nave);
}



// ------------------------- Exibicoes -----------------------------

void exibePontuacaoParcial(ALLEGRO_FONT *fonte, int pontuacao){
	char textoPontuacao[100];
	sprintf(textoPontuacao, "%d", pontuacao);	
	al_draw_text(fonte, al_map_rgb(255, 255, 255), 10,10, 0, textoPontuacao);
}


int obterRecorde(){
	FILE *arq = NULL;
    int recorde = 0;
    arq = fopen("recorde.txt", "r");
    
	if(arq == NULL){
		return 0;
	}
	
    while(feof(arq) == 0) {
       fscanf(arq, "%d", &recorde);
    }
    fclose(arq);
    return recorde;

}

void escreverRecorde(int recorde){
	FILE *arq;

	arq = fopen("recorde.txt", "w");
    fprintf(arq, "%d", recorde);
    fclose(arq); 
	return;
}

void exibePontuacaoFinal( ALLEGRO_FONT *fonte, int pontuacao){
	al_clear_to_color(COR_CENARIO);

	char textoPontuacao[100],  textoRecorde[100];	
	int recorde = obterRecorde();


	if(recorde < pontuacao){
		sprintf(textoRecorde, "NOVO RECORDE!");	
		escreverRecorde(pontuacao);
	} else {
		sprintf(textoRecorde, "Recorde: %d",recorde);	
	}

	sprintf(textoPontuacao, "Pontuacao: %d", pontuacao);	
	al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/3, ALLEGRO_ALIGN_CENTER, textoPontuacao);
	al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, textoRecorde);

	
	//reinicializa a tela
	al_flip_display();	
    al_rest(3);		
}





// ------------------------------ jogo -------------------------------






int main(int argc, char **argv){
	srand(time(NULL));
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *arial32 = al_load_font("arial.ttf", 32, 1);   
	if(arial32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
   


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());	

	//----------------------- rotinas de inicializacao ---------------------------------------

	initGlobais();

	Nave nave;
	initNave(&nave);

	Bloco bloco;
	initBloco(&bloco);

	Inimigo inimigos[QUANTIDADE_INIMIGOS];
	initInimigos(inimigos);



	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			desenhaCenario();
			
			atualizaNave(&nave);
			desenhaNave(nave);

			atualizaInimigos(inimigos);
			desenhaInimigos(inimigos);

			atualizaBloco(&bloco);
			desenhaBloco(bloco);

			exibePontuacaoParcial(arial32, nave.pontuacao);

			validaColisaoTiro(&nave, bloco, inimigos);
			validaColisaoInimigos(inimigos, bloco);
			playing = !validaColisaoNave(nave, bloco, inimigos); 

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_KEY_UP ) {
			manipulaEventoTeclado(ev.type, ev.keyboard.keycode,&nave );
		}


	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	exibePontuacaoFinal(arial32, nave.pontuacao);


	al_destroy_bitmap(nave.imagem);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}