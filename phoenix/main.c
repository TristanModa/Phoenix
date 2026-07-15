#include <stdio.h>
#include <vulfEngine.h>

void init() { printf("Init\n"); }
void destroy() { printf("Destroy\n"); }
void update() { printf("Update\n"); }
void render() { printf("Render\n"); }

int main(void) {
    // Create and run the application
    Application_create("Phoenix", (AppVersion){ 0, 0, 0 },
        init,
        destroy,
        update,
        render);
    Application_run();
}