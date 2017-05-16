#include "timer.h"
#include "recipe.h"

/* Main function of project 2. Kicks off execution to runRecipes function
   after basic init of pins on QNX. */
int main(int argc, char *argv[]) {
    pwmInit();
    runRecipes();
    return 0;
}
