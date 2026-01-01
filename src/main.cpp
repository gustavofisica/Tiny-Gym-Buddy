/*
 * Tiny Gym Buddy - MVP 2: Exercise Counter
 * 
 * Conta repetições de exercícios detectando ciclos de movimento (ON→OFF).
 * 
 * Funcionalidades:
 *   - Detecção de movimento via câmera (AIRecognition)
 *   - Contagem de transições MOVIMENTO→PARADO
 *   - Validação temporal (rejeita < 300ms ou > 3s)
 *   - LED RGB: Azul=idle, Vermelho=movimento
 * 
 * Hardware: Unihiker K10 (ESP32-S3, GC2145 camera)
 */

#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10 k10;
AIRecognition ai;
uint8_t screen_dir = 2;

// Configurações de detecção
const int MOTION_THRESHOLD = 50;  // Sensibilidade (10-200)

// Configurações de validação temporal
const unsigned long MIN_INTERVAL = 300;   // Mínimo 300ms entre repetições
const unsigned long MAX_INTERVAL = 3000;  // Máximo 3s entre repetições

// Estado do contador
int repetitionCount = 0;
bool lastMovementState = false;
unsigned long lastTransitionTime = 0;
bool validMovement = false;

// Declaração antecipada
void updateDisplay();

void setup() {
    // Inicializar hardware
    k10.begin();
    k10.initScreen(screen_dir);
    ai.initAi();
    
    // Configurar câmera
    k10.initBgCamerImage();
    k10.setBgCamerImage(false);
    k10.creatCanvas();
    
    // Ativar detecção de movimento
    ai.switchAiMode(ai.NoMode);
    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Move);
    ai.setMotinoThreshold(MOTION_THRESHOLD);
    
    // Display inicial
    updateDisplay();
    
    // LED azul = sistema pronto
    k10.rgb->write(-1, 0x0000FF);
    
    delay(1000);  // Mostrar tela inicial
}

void loop() {
    // Detectar movimento atual
    bool currentMovement = ai.isDetectContent(AIRecognition::Move);
    unsigned long now = millis();
    
    // Detectar transição MOVIMENTO → PARADO (fim de uma repetição)
    if (lastMovementState && !currentMovement) {
        unsigned long interval = now - lastTransitionTime;
        
        // Validação temporal: aceitar apenas intervalos entre 300ms e 3s
        if (interval > MIN_INTERVAL && interval < MAX_INTERVAL) {
            repetitionCount++;
            updateDisplay();
        }
        
        lastTransitionTime = now;
        validMovement = false;
    }
    
    // Detectar transição PARADO → MOVIMENTO (início de uma repetição)
    if (!lastMovementState && currentMovement) {
        lastTransitionTime = now;
        validMovement = true;
    }
    
    // Feedback visual com LED
    if (currentMovement) {
        k10.rgb->write(-1, 0xFF0000);  // Vermelho = movimento
    } else {
        k10.rgb->write(-1, 0x0000FF);  // Azul = parado
    }
    
    lastMovementState = currentMovement;
    delay(50);  // ~20 FPS
}

void updateDisplay() {
    k10.creatCanvas();
    k10.setScreenBackground(0xFFFFFF);
    
    // Título
    k10.canvas->canvasText("Tiny Gym Buddy", 0, 0x0000FF);
    
    // Contador de repetições (grande, centralizado)
    char counterText[32];
    sprintf(counterText, "Reps: %d", repetitionCount);
    k10.canvas->canvasText(counterText, 2, 0x000000);
    
    // Status
    if (validMovement) {
        k10.canvas->canvasText("Status: MOVING", 4, 0xFF0000);
    } else {
        k10.canvas->canvasText("Status: READY", 4, 0x00FF00);
    }
    
    k10.canvas->updateCanvas();
}