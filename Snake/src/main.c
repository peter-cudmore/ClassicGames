#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <sdl2/SDL.h>

enum GameState {
	IS_RUNNING,
	GAME_OVER,
	EXITING
};

enum GameState game_state = IS_RUNNING;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

enum directions {UP, DOWN, LEFT, RIGHT};
enum directions direction = LEFT;


#define square_size  16
#define maxRows  30
#define maxCols  40
int score = 0;
#define menuHeight  20
#define window_height (square_size*maxRows + menuHeight);


struct SnakeElement {
	int row;
	int col;
	void* next;
};

struct SnakeElement* snake;

int food_row = 0;
int food_col = 0;

void place_food(void) {
	
	struct SnakeElement* nextElement;

	while (SDL_TRUE) {
		nextElement = snake;
		food_row = rand() % maxRows;
		food_col = rand() % maxCols;

		// Loop through each snake element and if.
		while (nextElement != NULL) {
			if ((food_col != nextElement->col) &&
				(food_row != nextElement->row)) {
				return;
			}
			nextElement = (struct SnakeElement*) nextElement->next;
		}
	}
}

void move_snake(void){
	int nextRow = snake->row;
	int nextCol = snake->col;
	struct SnakeElement* body_segment = snake;
	struct SnakeElement *tail_element;

	switch (direction)
	{
	case UP:  nextRow--; break;
	case DOWN: nextRow++; break;
	case LEFT: nextCol--; break;
	case RIGHT: nextCol++; break;
	}

	if ((nextRow < 0) || (nextCol < 0) ||
		(nextRow >= maxRows) || (nextCol >= maxCols)) {

		game_state = GAME_OVER;
		printf("Out of bounds: %i, %i \n", nextRow, nextCol);
		return;
	}

	// Get the tail
	while (body_segment->next != NULL) {
		if ((body_segment->row == nextRow) && (body_segment->col == nextCol)) {
			game_state = GAME_OVER;
			return;
		}
		else {
			body_segment = (struct  SnakeElement*)  body_segment->next;
		}
	}
	
	if ((nextRow == food_row) && (nextCol == food_col)) {
		score++;
		
		body_segment = snake;
		
		snake = (struct SnakeElement*) malloc(sizeof(struct SnakeElement));
		if (snake == NULL) {
			printf("Out of Memory\n");
			exit(-1);
		}

		snake->next = body_segment;
		snake->row = nextRow;
		snake->col = nextCol;
		
		place_food();

	}
	else if (snake->next == NULL) {
		snake->col = nextCol;
		snake->row = nextRow;
	} else {
		tail_element = body_segment;

		body_segment = snake;
		
		while (body_segment->next != tail_element) 
		{ 
			body_segment = body_segment->next; 
		}

		body_segment->next = NULL;
		tail_element->row = nextRow;
		tail_element->col = nextCol;
		tail_element->next = snake;
		snake = tail_element;
	}
}


void handle_input(void) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) { 
			game_state = EXITING;
			return;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				game_state = EXITING;
				return;
			case SDLK_UP:
				if (direction != DOWN) {
					direction = UP;
					return;
				}
				break;
			case SDLK_DOWN:
				if (direction != UP) {
					direction = DOWN;
					return;
				}
				break;
				
			case SDLK_LEFT:
				if (direction != RIGHT) {
					direction = LEFT;
					return;
				}
				break;
			case SDLK_RIGHT:
				if (direction != LEFT) {
					direction = RIGHT;
					return;
				}
				break;
			default:
				break;
			}
		}
	}
}


void delete_element(struct SnakeElement *element) {
	
	if (element == NULL) {
		return;
	} 
	
	if (element->next != NULL) {
		delete_element(element->next);
		element->next = NULL;
	}
	free(element);
}

void init_game(void) {

	if (snake != NULL) {
		delete_element(snake->next);
		snake->next = NULL;
	}
	else {
		snake = (struct SnakeElement*) malloc(sizeof(struct  SnakeElement));
		
		if (snake == NULL) {
			printf("Could not allocate memory");
			exit(-1);
		}
		
		snake->next = NULL;
	}

	snake->row = maxRows / 2;
	snake->col = maxCols / 2;
	direction = LEFT;

	place_food();

}

void reset_game(void) {

}

void run_menu(void) {


}

void draw_square() {


}

SDL_bool init3d(void) {
	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("Snake", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
	if (window == NULL) { printf("Could not create window.\n"); exit(-1); }

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		printf("Could not get GL context\n");
		exit(-1);
	}
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	SDL_GL_SetSwapInterval(1);
	glViewport(0, 0, 640, 480);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
}

void init2d(void) {

	window = SDL_CreateWindow("Snake", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
	if (window == NULL) { printf("Could not create window.\n"); exit(-1); }
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) { printf("Could not get a renderer\n"); exit(-1); }
}

void Draw2d(void) {
	
	
	SDL_Rect painter_rect;
	struct SnakeElement* body_elements = snake;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);



	// draw bounds
	SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
	painter_rect.h = maxRows*square_size + menuHeight;
	painter_rect.w = maxCols*square_size + menuHeight;
	painter_rect.x = 0;
	painter_rect.y = 0;
	SDL_RenderDrawRect(renderer, &painter_rect);

	// set grid size
	painter_rect.h = square_size;
	painter_rect.w = square_size;

	// place food

	painter_rect.x = food_col * square_size + menuHeight;
	painter_rect.y = food_row * square_size + menuHeight;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &painter_rect);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	while (body_elements != NULL) {
		painter_rect.x = body_elements->col * square_size + menuHeight;
		painter_rect.y = body_elements->row * square_size + menuHeight;

		SDL_RenderFillRect(renderer, &painter_rect);
		body_elements = (struct SnakeElement*) body_elements->next;
	}

	SDL_RenderPresent(renderer);
}


int
main(int argc, char** argv)
{
	// Initialize screen
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Cound not initialise SDL2.\n");
		exit(-1);
	}
	
	init2d();

	game_state = IS_RUNNING;

	init_game();

	while (game_state != EXITING) { 

		if (game_state == IS_RUNNING) {
			handle_input();
			move_snake();
			Draw2d();
		}
		else
		{
			run_menu();
			
		}

		SDL_Delay(125);
	}

	
//	SDL_GL_DeleteContext(context);
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	return 0;
}
