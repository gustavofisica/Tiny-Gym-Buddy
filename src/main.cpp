#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2;  // Orientação da tela

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0xFFFFFF);  // Fundo branco
    k10.canvas->canvasText("Hello K10!", 1, 0x0000FF);  // Texto azul
    k10.canvas->canvasText("Tiny Gym Buddy", 2, 0x000000);  // Texto preto
    k10.canvas->updateCanvas();
    
    // LED azul = sistema iniciado
    k10.rgb->write(-1, 0x0000FF);
}

void loop() {
    // Aguardando implementação
    delay(100);
}
