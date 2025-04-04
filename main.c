#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tinyexpr.h"


char input[256];

void fstring() {
    printf("Enter a function (example: x^2 + 3*x): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
}

int chooseriemann(){
    char aux[256];
    int choice;
    printf("Choose what riemann sum you want to use\n");
    printf("1. High riemann sum\n");
    printf("2. Low riemann sum\n");
    printf("3. Middle riemann sum\n");
    printf("Enter a number from 1 to 3: ");
    scanf("%d", &choice);
    return choice;
}


double f(double x) {
    te_variable vars[] = { {"x", &x} };
    int err;
    te_expr *compiledExpr = te_compile(input, vars, 1, &err);

    if (compiledExpr) {
        double result = te_eval(compiledExpr);
        te_free(compiledExpr);
        return result;
    } else {
        printf("Error parsing expression at position: %d\n", err);
        return 0;
    }
}

void drawline(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color){
    int i;
    double x = x1 - x0; 
	double y = y1 - y0; 
	double length = sqrt( x*x + y*y ); 
	double addx = x / length; 
	double addy = y / length; 
	x = x0; 
	y = y0; 
	
	for ( i = 0; i < length; i += 1) { 
		SDL_Rect point = {x, y, 1, 1};
        SDL_FillRect(surface, &point, color);
		x += addx; 
		y += addy; 
		
	} 
}

void drawfunction(SDL_Surface* surface, int width, int height, int startx, int starty, int scaling, int invscaling) {
    int prevy = starty;
    int prevx = startx;
    SDL_Rect yaxis = {startx, (height - starty-height/2), 2, height/2};
    SDL_Rect xaxis = {startx, height - starty, width/2, 2};
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &yaxis, white);
    SDL_FillRect(surface, &xaxis, white);
    for (int i = 0; i < width/2; i++){
        //Displaced by startx
        int x = i;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        int y = height - (scaling*(f(x))/invscaling + starty);
        //(x, y, width, height)
        SDL_Rect rect = {x + startx, y, 1, 1};
        SDL_FillRect(surface, &rect, white);
        //Fill the gaps between the points
        drawline(surface, prevx, prevy, x + startx, y, white);
        prevx = x + startx;
        prevy = y;
    }
}

void drawlowriemann(SDL_Surface* surface, int width, int height, int startx, int starty, int scaling, int invscaling, double (*func)(double), double a, double b, int n) {
    Uint32 lightblue = SDL_MapRGB(surface->format, 172, 216, 230);
    //SDL_Rect yaxis = {startx, (height - starty-350), 1, 350};
    double dx = round((b - a)/(double)n);
    if(dx < 1){
        dx = 1;
    }
    for (int i = 0; i < n; i++){
        //Displaced by startx
        double x = a + i*dx;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        double h = func(x);
        double y = ceil(height - (scaling*h/invscaling + starty));
        //(x, y, width, height)
        if(x < b){
            SDL_Rect rect = {x+startx, y, dx, scaling*h/invscaling};
            SDL_FillRect(surface, &rect, lightblue);
        }
    }
}

void drawhighriemann(SDL_Surface* surface, int width, int height, int startx, int starty, int scaling, int invscaling, double (*func)(double), double a, double b, int n) {
    Uint32 lightblue = SDL_MapRGB(surface->format, 172, 216, 230);
    //SDL_Rect yaxis = {startx, (height - starty-350), 1, 350};
    double dx = round((b - a)/(double)n);
    if(dx < 1){
        dx = 1;
    }
    for (int i = 0; i < n; i++){
        //Displaced by startx
        double x = a + i*dx;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        double h = func(x+dx);
        double y = ceil(height - (scaling*h/invscaling + starty));        //(x, y, width, height)
        if(x < b){
            SDL_Rect rect = {x+startx, y, dx, scaling*h/invscaling};
            SDL_FillRect(surface, &rect, lightblue);
        }
    }
}

void drawmiddleriemann(SDL_Surface* surface, int width, int height, int startx, int starty, int scaling, int invscaling, double (*func)(double), double a, double b, int n) {
    Uint32 lightblue = SDL_MapRGB(surface->format, 172, 216, 230);
    //SDL_Rect yaxis = {startx, (height - starty-350), 1, 350};
    double dx = round((b - a)/(double)n);
    if(dx < 1){
        dx = 1;
    }
    for (int i = 0; i < n; i++){
        //Displaced by startx
        double x = a + i*dx;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        double h = func(x+dx*0.5);
        double y = ceil(height - (scaling*h/invscaling + starty));        //(x, y, width, height)
        if(x < b){
            SDL_Rect rect = {x+startx, y, dx, scaling*h/invscaling};
            SDL_FillRect(surface, &rect, lightblue);
        }
    }
}

double lowriemannsum(double (*func)(double), double a, double b, int n){
    double sum = 0.0;
    double dx = (b - a)/n;
    for (double i = 0.0; i < n; i++){
        sum += (func(a + i*dx)*dx);
    }
    return sum;
}

double highriemannsum(double (*func)(double), double a, double b, int n){
    double sum = 0.0;
    double dx = (b - a)/n;
    for (double i = 0.0; i < n; i++){
        sum += (func(a + dx + i*dx)*dx);
    }
    return sum;
}

double middleriemannsum(double (*func)(double), double a, double b, int n){
    double sum = 0.0;
    double dx = (b - a)/n;
    for (double i = 0.0; i < n; i++){
        sum += (func((a+a + dx + i*dx+i*dx)*0.5)*dx);
    }
    return sum;
}

int main(int argc, char* argv[]) {
    int width = 900;
    int height = 700;
    char text[256];

    int a = 100;
    int b = 250;
    int n = 7;

    fstring();

    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("RiemannSum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    TTF_Font* font = TTF_OpenFont("assets/Semi-Coder-Regular.otf", 12);
    if (font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    int startx = width / 4;
    int starty = height / 4;

    //Scaling factor to draw a more visual function

    int invscaling = 1;
    int scaling = 1;
    if(f(width/2)>height/2){
        invscaling = f(width/2) / (height/2);
    }else if (f(width/2)<height/4)
    {
        scaling = 8;
    }
    
    // Draw the function
    drawfunction(surface, width, height, startx, starty, scaling, invscaling);

    switch (chooseriemann())
    {
    case 1:
        drawhighriemann(surface, width, height, startx, starty, scaling, invscaling, f, a, b, n);
        break;
    case 2:
        drawlowriemann(surface, width, height, startx, starty, scaling, invscaling, f, a, b, n);
        break;
    case 3:
        drawmiddleriemann(surface, width, height, startx, starty, scaling, invscaling, f, a, b, n);
        break;
    default:
        break;
    }

    
    double highsum = highriemannsum(f, a, b, n);
    double lowsum = lowriemannsum(f, a, b, n);
    double middlesum = middleriemannsum(f, a, b, n);
    
    // Render text
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    sprintf(text, "Riemann Sum (%d, %d) - H:%f M:%f L:%f", a, b, highsum, middlesum, lowsum);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface == NULL) {
        printf("TTF_RenderText_Solid: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h}; // Position and size of the text
    SDL_BlitSurface(textSurface, NULL, surface, &textRect);

    SDL_FreeSurface(textSurface);

    SDL_UpdateWindowSurface(window);

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { // Check for the "X" button click
                running = 0; // Exit the loop
            }
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}