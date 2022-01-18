#include <stdio.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;


const int VELOCIDADE_NAVE = 6;
const int TAMANHO_IMAGEM_NAVE = 190;

ALLEGRO_COLOR COR_CENARIO;

typedef struct Circulo {
	int xCentro, yCentro;
	int raio;
} Circulo;

typedef struct Retangulo {
	int x, y;
	int largura, altura;
} Retangulo;

typedef struct Nave {
	int x, y;
	int dirX, dirY;
	int velocidade;
	int largura, altura;
	ALLEGRO_BITMAP *imagem;
	Circulo hitbox;
} Nave;

typedef struct Bloco {
	int x,y;
	int largura, altura;
	ALLEGRO_COLOR cor;
} Bloco;

void initGlobais(){
	COR_CENARIO = al_map_rgb(41,33,50);
}

void initNave(Nave * nave){
	nave->largura = 108;
	nave->altura = 90;

	nave->x = 10 + nave->largura;
	nave->y = SCREEN_H/2;

	nave->dirX = 0;
	nave->dirY = 0;
	nave->velocidade = VELOCIDADE_NAVE;

	nave->imagem = al_load_bitmap("swordfish.png");

	nave->hitbox.raio = nave->largura < nave->altura ? (nave->largura / 2) : (nave->altura / 2);
	nave->hitbox.xCentro = nave->x + (nave->largura / 2);
	nave->hitbox.yCentro = nave->y + (nave->altura / 2);

}


void initBloco(Bloco * bloco){
	bloco->x = ( 1 * SCREEN_W ); //+ rand() % (SCREEN_W);
	bloco->y = rand() % ( 4 * SCREEN_H /5);
	bloco->largura =  SCREEN_W + rand() % (SCREEN_W); 
	bloco->altura = SCREEN_H/5 +  rand() % (2 * SCREEN_H/5);
	bloco->cor = al_map_rgb( 255,255,255);
}

void desenhaCenario() {
	al_clear_to_color(COR_CENARIO);
}

void desenhaNave(Nave nave){
	al_draw_scaled_bitmap(nave.imagem,
		0, 0,
	 	al_get_bitmap_width(nave.imagem),al_get_bitmap_height(nave.imagem),
		nave.x, nave.y,
		nave.largura,nave.altura,
		0
	);
}

void desenhaBloco(Bloco bloco){
	al_draw_filled_rectangle(bloco.x, bloco.y, bloco.x + bloco.largura, bloco.y + bloco.altura, bloco.cor );
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
		
}

void atualizaBloco(Bloco * bloco){
	bloco->x -= 3;

	if( ( bloco->x + bloco->largura)  < 0 ){
		initBloco(bloco);
	}
}


int validaColisaoCirculoRetangulo(Circulo circulo, Retangulo retangulo){

}
int  validaColisaoNaveBloco(Nave nave, Bloco bloco){
	



}


 
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
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
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
			atualizaBloco(&bloco);
			
			desenhaNave(nave);
			desenhaBloco(bloco);

			playing = !validaColisaoNaveBloco(nave, bloco); 
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_W:
					nave.dirY--;
				break;
				case ALLEGRO_KEY_S:
					nave.dirY++;
				break;
				case ALLEGRO_KEY_A:
					nave.dirX--;
				break;
				case ALLEGRO_KEY_D:
					nave.dirX++;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_W:
					nave.dirY++;
				break;
				case ALLEGRO_KEY_S:
					nave.dirY--;
				break;
				case ALLEGRO_KEY_A:
					nave.dirX++;
				break;
				case ALLEGRO_KEY_D:
					nave.dirX--;
				break;
			}
		}

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}