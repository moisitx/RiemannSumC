# Riemann Sum Visualizer

This project is a C program that uses **SDL2** and **SDL_ttf** to visualize mathematical functions and compute Riemann sums (upper, lower, and middle).

The application features:
- Graphing functions on a scaled Cartesian coordinate system.
- Visualizing Riemann sums with dynamically generated rectangles.
- Displaying the computed Riemann sums directly on the window.

The program is designed to help users better understand the concept of Riemann sums and their relationship to integral calculus. It includes customization options for the range of integration, number of partitions, and the function to be visualized.

# How to try

Download the zip from the repository and run the executable file. Due to the function being scaled so it is more appealing the number of rectangles shown might not match the number of rectangles chosen for the sum, that's because SDL can draw 1 pixel wide rectangles, and there might not be enough pixels for every single one.
