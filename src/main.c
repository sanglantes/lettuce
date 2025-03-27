#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "utils.h"
#include "vector.h"
#include "lagrange.h"
#include "gram_schmidt.h"
#include "babai.h"

#include "SDL3_gfxPrimitives.h"

#define SDL_FLAGS (SDL_INIT_VIDEO)

#define WINDOW_TITLE "Lettuce"
#define WIDTH	  1000
#define HEIGHT	800

uint16_t GRID = 30;

struct Config {
	SDL_Window* 	window;
	SDL_Renderer* 	renderer;
	SDL_Event	event;
  	SDL_Texture*  	tex;
	bool		running;

	vector_t*	b1;
	vector_t* 	b2;
 	char*           bstr;
	bool		verbose;

	bool		fundamental_domain;
	bool		reduce;
	bool		show_points;
  	bool            cvp;
  	vector_t        cvp_coords;

	vector_t*	precomp_points;
  	float           zoom;
  	vector_t        origin;
};

bool init_sdl(struct Config *c, bool verbose);
void sdl_kill_wrapper(struct Config *c, bool verbose);
void sdl_events(struct Config *c);
vector_t sdl_origin_translation(vector_t* v, float zoom);
SDL_Texture* sdl_render_points(struct Config *c);
void sdl_draw(struct Config *c);
void sdl_run(struct Config *c);

bool init_sdl(struct Config *c, bool verbose) {
	if (!SDL_Init(SDL_FLAGS)) {
sdl_err:
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		return false;
	}

	c->window = SDL_CreateWindow(WINDOW_TITLE, WIDTH, HEIGHT, 0);
	if (!c->window) { goto sdl_err; }

	c->renderer = SDL_CreateRenderer(c->window, NULL);
	if (!c->renderer) { goto sdl_err; }

	if (verbose) {
		puts("[INFO] Successfully intitialized SDL3.");
	}

	return true;
}

void sdl_kill_wrapper(struct Config *c, bool verbose) {
	if (c->window) {
		SDL_DestroyWindow(c->window);
		c->window = NULL;
	}
	
	if (c->renderer) {
		SDL_DestroyRenderer(c->renderer);
		c->renderer = NULL;
	}
	
	if (verbose) {
		puts("[INFO] Successfully killed SDL3.");
	}

	SDL_Quit();
}

void sdl_events(struct Config *c) {
	while (SDL_PollEvent(&c->event)) {
		switch (c->event.type) {
			case SDL_EVENT_QUIT:
				c->running = false;
				break;

			case SDL_EVENT_KEY_DOWN:
				switch (c->event.key.scancode) {
					case SDL_SCANCODE_ESCAPE:
						c->running = false;
					break;

					case SDL_SCANCODE_F:
						c->fundamental_domain = !(c->fundamental_domain);
					break;

				          case SDL_SCANCODE_R:
				            lagrange_reduction(c->b1, c->b2);
				            if (c->verbose) {
				                printf("[INFO] Lagrange reduction yielded b1 = (%Lf, %Lf), b2 = (%Lf, %Lf).\n",
				                       c->b1->e1, c->b1->e2, c->b2->e1, c->b2->e2);
				            }
				
				            c->reduce = true;
				            int half_grid = GRID / 2;
				            uint32_t ctr = 0;
				            for (int i = -half_grid; i < half_grid; i++) {
				                for (int j = -half_grid; j < half_grid; j++) {
				                    vector_t vt = vector_vadd(vector_smul(i, *(c->b1)), vector_smul(j, *(c->b2)));
				                    c->precomp_points[ctr] = sdl_origin_translation(&vt, c->zoom);
				                    ctr++;
				                }
				            }

				            c->tex = sdl_render_points(c);
				            SDL_RenderTexture(c->renderer, c->tex, NULL, NULL);
				            break;
					
					case SDL_SCANCODE_H:
						c->show_points = !(c->show_points);
					break;
          
				  case SDL_SCANCODE_C:
				      float x = 0.0f;
				      float y = 0.0f;
				      SDL_GetMouseState(&x, &y);
				      
				      vector_t t = {(x - WIDTH/2) / c->zoom, -(y - HEIGHT/2) / c->zoom};
				      vector_t cvp_vector = babai2_cvp(*c->b1, *c->b2, t);
				      
				      c->cvp_coords = sdl_origin_translation(&cvp_vector, c->zoom); // fix this if you can!
				      c->cvp = true;
				      break;

					default:
					break;
				}
				break;

			default:
			break;
		}
	}
}

vector_t sdl_origin_translation(vector_t* v, float zoom) {
	vector_t rop_vector;
	rop_vector.e1 = (v->e1*zoom + WIDTH/2);
	rop_vector.e2 = (HEIGHT/2 - v->e2 * zoom);

	return rop_vector;
}

void sdl_draw(struct Config *c) {
	SDL_SetRenderDrawColor(c->renderer, 255, 255, 255, 255);
	SDL_RenderClear(c->renderer);

	SDL_SetRenderDrawColor(c->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderLine(c->renderer, 0, HEIGHT/2, WIDTH, HEIGHT/2); // x-axis
	SDL_RenderLine(c->renderer, WIDTH/2, HEIGHT, WIDTH/2, 0); // y-axis
  
	vector_t translate_b1 = sdl_origin_translation(c->b1, c->zoom);
	vector_t translate_b2 = sdl_origin_translation(c->b2, c->zoom);
	uint8_t vector_rcolor = (c->fundamental_domain) ? 200 : 0;
	aalineRGBA(c->renderer, WIDTH/2, HEIGHT/2, translate_b1.e1, translate_b1.e2, vector_rcolor, 0, 0, 255); // b1
	aalineRGBA(c->renderer, WIDTH/2, HEIGHT/2, translate_b2.e1, translate_b2.e2, vector_rcolor, 0, 0, 255); // b2

  if (c->show_points) {
    SDL_RenderTexture(c->renderer, c->tex, NULL, NULL);
  }
	
  if (c->fundamental_domain) {
      vector_t bsumt = vector_vadd(*(c->b1), *(c->b2));
      vector_t bsum = sdl_origin_translation(&bsumt, c->zoom);
  
      vector_t translate_b1 = sdl_origin_translation(c->b1, c->zoom);
      vector_t translate_b2 = sdl_origin_translation(c->b2, c->zoom);
  
      aalineRGBA(c->renderer, translate_b1.e1, translate_b1.e2, bsum.e1, bsum.e2, vector_rcolor, 0, 0, 255);
      aalineRGBA(c->renderer, translate_b2.e1, translate_b2.e2, bsum.e1, bsum.e2, vector_rcolor, 0, 0, 255);
  
      int16_t vx[] = {c->origin.e1, translate_b1.e1, bsum.e1, translate_b2.e1};
      int16_t vy[] = {c->origin.e2, translate_b1.e2, bsum.e2, translate_b2.e2};
      filledPolygonRGBA(c->renderer, vx, vy, 4, vector_rcolor, 0, 0, 50);
  
      char buf[50];
      sprintf(buf, "%d", (int)abs(vector_det(*c->b1, *c->b2)));
      vector_t center = {(c->b1->e1 + c->b2->e1) / 2, (c->b1->e2 + c->b2->e2) / 2};
      vector_t center_t = sdl_origin_translation(&center, c->zoom);
      stringRGBA(c->renderer, center_t.e1 - 12, center_t.e2, buf, 0, 0, 0, 255);
  }

  if (c->cvp) {
    aacircleRGBA(c->renderer, c->cvp_coords.e1, c->cvp_coords.e2, 10, 0, 0, 200, 255);
  }

	SDL_RenderPresent(c->renderer);
}

void sdl_run(struct Config *c) {
	c->running = true;

	while (c->running) {
		sdl_events(c);
		sdl_draw(c);

		SDL_Delay(16);
	}
}

SDL_Texture* sdl_render_points(struct Config *c) {
  SDL_Texture* point_texture = SDL_CreateTexture(c->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
  SDL_SetTextureBlendMode(point_texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(c->renderer, point_texture);

  SDL_SetRenderDrawColor(c->renderer, 0, 0, 0, 0);
  SDL_RenderClear(c->renderer);
  
  float scale = 2;
  scale += (c->zoom == 1.0f) ? 0 : c->zoom * 0.05;

  for (int i = 0; i < (GRID*GRID) - 1; i++) {
  	vector_t t = c->precomp_points[i];
    vector_t t3 = c->precomp_points[(i + GRID)%(GRID*GRID)];
    
  	filledCircleRGBA(c->renderer, t.e1, t.e2, scale, 0, 10, 0, 255);
    
    if ((i + 1) % GRID != 0) {
      vector_t t2 = c->precomp_points[i + 1]; 
      aalineRGBA(c->renderer, t.e1, t.e2, t2.e1, t2.e2, 10, 10, 100, 30);
    }
    aalineRGBA(c->renderer, t.e1, t.e2, t3.e1, t3.e2, 10, 10, 100, 30);
  }

  SDL_SetRenderTarget(c->renderer, NULL);
  return point_texture;
}

int main(int argc, char* argv[]) {
	char* str_basis = NULL;
	long double basis[2][2];
  float zoom = 0;
  bool help = false;

	bool verbose = false;
	int opt;

	while ((opt = getopt(argc, argv, "b:c:vz:h")) != -1) {
		switch (opt) {
			case 'b':
				str_basis = optarg;
				break;

			case 'v':
				verbose = true;
				break;

			case 'c':
				GRID = (uint16_t)atoi(optarg);
				break;

      case 'z':
        zoom = atof(optarg);
        break;

      case 'h':
        printf("Usage: %s [-vh] <-b BASIS> [-z ZOOM] [-c COUNT]\n\n", argv[0]);
        printf("Visualize lattices in R^2.\n\n");

        puts("Options:");
        printf("\t-v\tEnable verbose mode (default: 0).\n");
        printf("\t-b x1,y1,x2,y2\n\t\tSet basis vectors.\n");
        printf("\t-c C\tSet grid size of lattice points to draw (default: 30)\n");
        printf("\t-z Z\tSet positive zoom factor (default: 1).\n");
        printf("\t-h\tPrints this message.\n\n");

        puts("Examples:");
        printf("\t%s -vb 1,2,3,4 -c 15 -z 30\n", argv[0]);
        printf("\t%s -b 100,50,-30,-20\n\n", argv[0]);

        puts("Notes:");
        puts("The use of -z and -c are computationally expensive.");
        puts("See README.md for keyboard controls inside the application.");
        exit(0);
        break;
	default:
use_err:
				fprintf(stderr,
            "Usage: %s [-vh] <-b BASIS> [-z ZOOM] [-c COUNT]\nType %s -h for help.\n", argv[0], argv[0]);
				exit(EXIT_FAILURE);
        break;
		}
	}

	if (str_basis == NULL || strcount(str_basis, ',') != 3) { goto use_err; }
  	char* token = strtok(str_basis, ",");
	for (uint8_t i = 0; i < 4; i++) {
		basis[i / 2][i & 1] = strtold(token, NULL);
		token = strtok(NULL, ",");
	}
	vector_t b1 = vector_init(basis[0][0], basis[0][1]);
	vector_t b2 = vector_init(basis[1][0], basis[1][1]);
  	char** vector_string = string_to_vectorf(basis[0][0], basis[0][1], basis[1][0], basis[1][1]);

	if (verbose) {
		printf("[INFO] Basis vectors set b1 = (%Lf, %Lf), b2 = (%Lf, %Lf).\n", b1.e1, b1.e2, b2.e1, b2.e2);
		if (ldependent(b1, b2)) {
			puts("[WARNING] Vectors are linearly dependent. They will form a sublattice.");
		}
		if (!(vector_det(b1, b2) == 1 || vector_det(b1, b2) == (-1))) {
			puts("[WARNING] The lattice matrix is not unimodular. b1, b2 will not form a basis over Z^2.");
		}
	}

	struct Config c = {0};
	if (!init_sdl(&c, verbose)) {
		return EXIT_FAILURE;
	}

	c.b1 = &b1;
	c.b2 = &b2;
  	c.zoom = zoom ? zoom : 1;

	vector_t* points = malloc(sizeof(vector_t) * (GRID*GRID));
	if (points == NULL) { 
		perror("[ERROR] -c malloc failure."); 
		return EXIT_FAILURE;
	}

	int half_grid = GRID / 2;
	uint32_t ctr = 0;
	for (int i = -half_grid; i < half_grid; i++) {
	    for (int j = -half_grid; j < half_grid; j++) {
	        vector_t vt = vector_vadd(vector_smul(i, b1), vector_smul(j, b2));
	        points[ctr] = sdl_origin_translation(&vt, c.zoom);
		ctr++;
	    }
	}

	c.precomp_points = points;
  
	c.verbose = verbose;

  	c.origin = sdl_origin_translation(&(vector_t){0, 0}, c.zoom);

  	c.tex = sdl_render_points(&c);

	sdl_run(&c);

	sdl_kill_wrapper(&c, verbose);

	free(points);

	return EXIT_SUCCESS;
}
