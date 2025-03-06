#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>



double f(double x) {
    return x*x;
}

void drawfunction(SDL_Surface* surface, int width, int height, int startx, int starty, int scalingx, int scalingy) {
    SDL_Rect yaxis = {startx, (height - starty-350), 1, 350};
    SDL_Rect xaxis = {startx, height - starty, 450, 1};
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &yaxis, white);
    SDL_FillRect(surface, &xaxis, white);
    for (int i = 0; i < 200; i++){
        //Displaced by startx
        int x = i*scalingx + startx;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        int y = height - ((f(x-startx))/scalingy + starty);
        //(x, y, width, height)
        SDL_Rect rect = {x, y, 1, 1};
        SDL_FillRect(surface, &rect, white);
    }
}

void drawriemann(SDL_Surface* surface, int width, int height, int startx, int starty, int scalingx, int scalingy, double (*func)(double), double a, double b, int n) {
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);
    //SDL_Rect yaxis = {startx, (height - starty-350), 1, 350};
    for (int i = 0; i < n; i++){
        double dx = (b - a)/n;
        if(dx < 1){
            dx = 1;
        }
        //Displaced by startx
        int x = a + i*dx + i*scalingx;
        //Displaced by starty and inverted the Y axis so it increases upwards (Since I want the Y to be drawn from the bottom, I substract the height minus the function value and draw it from the top)
        int h = func(x);
        int y = height - (h/scalingy + starty);
        //(x, y, width, height)
        if(x < b){
            SDL_Rect rect = {x+startx, y, dx, h/scalingy};
            SDL_FillRect(surface, &rect, white);
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
    int width = 800;
    int height = 600;
    char text[256];
    int a = 150;
    int b = 250;
    int n = 20;

    double highsum = highriemannsum(f, a, b, 100);
    double lowsum = lowriemannsum(f, a, b, 10000);
    double middlesum = middleriemannsum(f, a, b, 100);

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
    int scalingy = 200;
    int scalingx = 2;
    // Draw the function
    drawfunction(surface, width, height, startx, starty, scalingx, scalingy);
    drawriemann(surface, width, height, startx, starty, scalingx, scalingy, f, a, b, n);
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