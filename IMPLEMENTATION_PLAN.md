# Plano de Implementação: Contador de Exercícios Tiny Gym Buddy
## Unihiker K10 com PlatformIO + GitHub

---

## 📋 Sumário Executivo

**Objetivo:** Desenvolver um contador de exercícios (polichinelos) usando apenas a câmera da Unihiker K10, utilizando a biblioteca nativa `AIRecognition` para detecção de movimento.

**Restrições:**
- ❌ **NÃO** usar acelerômetro
- ✅ **SOMENTE** câmera (GC2145 2MP)
- ✅ Usar biblioteca nativa `AIRecognition` (detecção de movimento binária)
- ✅ Arduino C++ com PlatformIO no VSCode
- ✅ Controle de versão com GitHub

**Estratégia Principal:**
Utilizar `ai.isDetectContent(AIRecognition::Move)` para detectar ciclos de movimento (ON→OFF) e contar como repetições de exercício.

---

## 🔧 Especificações Técnicas da K10

### Hardware
| Componente | Especificação |
|-----------|---------------|
| **MCU** | ESP32-S3 N16R8, Xtensa LX7 dual-core @ 240MHz |
| **Memória** | 512KB SRAM, 8MB PSRAM, 16MB Flash |
| **Câmera** | GC2145 2MP, FOV 80° |
| **Display** | 2.8" 240x320 ILI9341 LCD |
| **RGB LED** | 3x WS2812 (feedback visual) |
| **Botões** | A, B, AB (controle do contador) |

### Software
- **Plataforma:** ESP32 firmware (NÃO é Linux como a M10)
- **Linguagens:** Arduino C++, MicroPython, Mind+
- **Bibliotecas Nativas:**
  - `unihiker_k10.h` - display, canvas, sensores, RGB LED
  - `AIRecognition.h` - detecção de movimento por IA

---

## 🛠️ Configuração do PlatformIO

### Passo 1: Instalação da Extensão
1. Abrir VSCode
2. Ir em Extensions (Ctrl+Shift+X)
3. Buscar "PlatformIO IDE"
4. Instalar a extensão oficial

### Passo 2: Criação do Projeto
⚠️ **IMPORTANTE:** Não é possível selecionar "unihiker_k10" diretamente na interface do PlatformIO!

**Procedimento correto:**
1. `Ctrl+Shift+P` → "PlatformIO: New Project"
2. Nome: `TinyGymBuddy`
3. Board: **Selecionar qualquer board DFRobot aleatória** (ex: "FireBeetle-ESP32")
4. Framework: `Arduino`
5. Location: `/home/gustavo/Github/Tiny-Gym-Buddy`
6. Clicar em "Finish"

### Passo 3: Configuração do `platformio.ini`
**Substituir TODO o conteúdo do `platformio.ini` por:**

```ini
[env:unihiker]
platform = https://github.com/DFRobot/platform-unihiker.git
board = unihiker_k10
framework = arduino
build_flags = 
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1
    -DModel=None
monitor_speed = 115200
```

### Passo 4: Download do SDK
1. Salvar o arquivo (`Ctrl+S`)
2. **Aguardar** download automático do `platform-unihiker` do GitHub
3. Verificar no terminal do PlatformIO: "Installing platform-unihiker..."
4. SDK inclui automaticamente `unihiker_k10.h` e `AIRecognition.h`

### Passo 5: Upload para K10
- **Compilar:** `Ctrl+Alt+B`
- **Upload:** `Ctrl+Alt+U`
- Conectar K10 via USB antes do upload

---

## 📂 Estrutura do Projeto

```
Tiny-Gym-Buddy/
├── .git/                     # Controle de versão Git
├── .gitignore                # Ignorar .pio/, .vscode/
├── platformio.ini            # Configuração do PlatformIO
├── README.md                 # Documentação do projeto
├── IMPLEMENTATION_PLAN.md    # Este documento
├── src/
│   └── main.cpp              # Código principal do contador
├── lib/                      # Bibliotecas customizadas (futuro)
│   └── README                # Placeholder
├── test/                     # Testes (futuro)
│   └── README                # Placeholder
└── docs/
    ├── API_REFERENCE.md      # Referência das APIs K10
    └── CALIBRATION.md        # Guia de calibração de threshold
```

---

## 🧠 Arquitetura do Código

### API de Detecção de Movimento

```cpp
#include "AIRecognition.h"

AIRecognition ai;

// Setup
ai.initAi();                                    // Inicializar módulo IA
ai.switchAiMode(ai.Move);                       // Ativar detecção de movimento
ai.setMotinoThreshold(50);                      // Sensibilidade (10-200)

// Loop
bool hasMovement = ai.isDetectContent(AIRecognition::Move);  // Retorna true/false
```

**⚠️ LIMITAÇÃO CRÍTICA:** A detecção retorna **APENAS booleano** (true/false), sem informações espaciais (onde o movimento ocorreu).

### Estratégia de Contagem

Como não temos dados espaciais (pose estimation), vamos contar **transições de estado**:

```
Ciclo de Polichinelo:
┌─────────────────────────────────────────────────┐
│ IDLE → MOVIMENTO → IDLE → MOVIMENTO → IDLE ... │
│  (1)      (2)       (3)      (4)       (5)      │
└─────────────────────────────────────────────────┘

Contador incrementa nas transições: (2)→(3) e (4)→(5)
```

**Lógica:**
- Movimento detectado (true) → LED vermelho
- Movimento parou (false) → LED azul + **incrementar contador**
- Validação temporal: rejeitar se intervalo < 300ms ou > 3s

### Máquina de Estados

```cpp
enum State {
    IDLE,               // Aguardando primeiro movimento
    MOVEMENT_DETECTED,  // Movimento ativo
    COUNTING            // Contando repetição
};

State currentState = IDLE;
unsigned long lastTransitionTime = 0;
int counter = 0;

void loop() {
    bool hasMovement = ai.isDetectContent(AIRecognition::Move);
    unsigned long now = millis();
    
    switch (currentState) {
        case IDLE:
            if (hasMovement) {
                currentState = MOVEMENT_DETECTED;
                lastTransitionTime = now;
            }
            break;
            
        case MOVEMENT_DETECTED:
            if (!hasMovement) {
                unsigned long interval = now - lastTransitionTime;
                if (interval > 300 && interval < 3000) {  // Validação temporal
                    counter++;
                    currentState = COUNTING;
                }
                lastTransitionTime = now;
            }
            break;
            
        case COUNTING:
            if (hasMovement) {
                currentState = MOVEMENT_DETECTED;
                lastTransitionTime = now;
            }
            break;
    }
}
```

---

## 📝 Exemplo de Código MVP

### Estrutura Básica (Hello K10)

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2;  // Orientação da tela

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.setScreenBackground(0xFFFFFF);  // Fundo branco
    k10.canvas->canvasText("UNIHIKER", 1, 0x0000FF);  // Texto azul
    k10.canvas->updateCanvas();
}

void loop() {
    // Vazio
}
```

### MVP: Detector de Movimento com LED

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10  k10;
uint8_t screen_dir = 2;
AIRecognition ai;

void setup() {
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
    ai.setMotinoThreshold(50);  // Threshold padrão
    
    // LED azul = idle
    k10.rgb->write(-1, 0x0000FF);
}

void loop() {
    if (ai.isDetectContent(AIRecognition::Move)) { 
        k10.rgb->write(-1, 0xFF0000);  // Vermelho = movimento
    } else {
        k10.rgb->write(-1, 0x0000FF);  // Azul = idle
    }
}
```

### MVP: Contador Básico

```cpp
#include "unihiker_k10.h"
#include "AIRecognition.h"

UNIHIKER_K10  k10;
uint8_t screen_dir = 2;
AIRecognition ai;

int counter = 0;
bool lastMovementState = false;
unsigned long lastTransitionTime = 0;

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    ai.initAi();
    
    k10.initBgCamerImage();
    k10.setBgCamerImage(false);
    k10.creatCanvas();
    
    ai.switchAiMode(ai.NoMode);
    k10.setBgCamerImage(true);
    ai.switchAiMode(ai.Move);
    ai.setMotinoThreshold(50);
    
    k10.rgb->write(-1, 0x0000FF);
    updateDisplay();
}

void loop() {
    bool currentMovement = ai.isDetectContent(AIRecognition::Move);
    unsigned long now = millis();
    
    // Detectar transição MOVIMENTO → PARADO
    if (lastMovementState && !currentMovement) {
        unsigned long interval = now - lastTransitionTime;
        
        // Validação temporal (300ms a 3s)
        if (interval > 300 && interval < 3000) {
            counter++;
            updateDisplay();
        }
        
        lastTransitionTime = now;
    }
    
    // Feedback visual
    if (currentMovement) {
        k10.rgb->write(-1, 0xFF0000);  // Vermelho
    } else {
        k10.rgb->write(-1, 0x0000FF);  // Azul
    }
    
    lastMovementState = currentMovement;
    delay(50);  // 20 FPS
}

void updateDisplay() {
    k10.creatCanvas();
    k10.setScreenBackground(0xFFFFFF);
    
    // Título
    k10.canvas->canvasText("Tiny Gym Buddy", 0, 0x000000);
    
    // Contador
    char buffer[32];
    sprintf(buffer, "Repetições: %d", counter);
    k10.canvas->canvasText(buffer, 2, 0x0000FF);
    
    k10.canvas->updateCanvas();
}
```

---

## 🌿 Workflow do GitHub

### Configuração Inicial

```bash
# Repositório já clonado
cd /home/gustavo/Github/Tiny-Gym-Buddy

# Verificar remote
git remote -v

# Criar branch develop
git checkout -b develop
git push -u origin develop
```

### Estratégia de Branches

```
main                 # Código estável, testado no hardware
├── develop          # Integração de features
│   ├── feature/movement-detection
│   ├── feature/counter-logic
│   ├── feature/display-ui
│   └── feature/button-controls
└── hotfix/...       # Correções urgentes
```

### Convenção de Commits

```
feat: Add movement detection with AIRecognition
fix: Correct counter increment logic
docs: Update calibration guide
refactor: Improve state machine readability
test: Add temporal validation tests
```

### Workflow de Desenvolvimento

1. **Criar feature branch:**
   ```bash
   git checkout develop
   git checkout -b feature/movement-detection
   ```

2. **Desenvolver e testar:**
   - Editar `src/main.cpp`
   - Compilar: `Ctrl+Alt+B`
   - Upload: `Ctrl+Alt+U`
   - Testar no hardware

3. **Commit incremental:**
   ```bash
   git add src/main.cpp
   git commit -m "feat: Implement basic movement detection"
   ```

4. **Merge para develop:**
   ```bash
   git checkout develop
   git merge feature/movement-detection
   git push origin develop
   ```

5. **Release para main:**
   ```bash
   git checkout main
   git merge develop
   git tag -a v1.0.0 -m "Release: MVP movement counter"
   git push origin main --tags
   ```

---

## 🧪 Estratégia de Testes e Calibração

### Fase 1: Validação de Movimento
**Objetivo:** Confirmar que `ai.isDetectContent(AIRecognition::Move)` funciona

**Testes:**
1. Upload do código MVP com LED
2. Mover mão na frente da câmera
3. **Esperado:** LED muda de azul para vermelho
4. **Se falhar:** Ajustar `ai.setMotinoThreshold(X)` (testar 30, 50, 100, 150)

### Fase 2: Calibração de Threshold
**Objetivo:** Encontrar sensibilidade ideal para polichinelos

**Procedimento:**
1. Criar código de teste com threshold variável:
   ```cpp
   int threshold = 50;
   
   void setup() {
       // ... config padrão ...
       ai.setMotinoThreshold(threshold);
   }
   
   void loop() {
       // Botão A: aumentar threshold (+10)
       if (k10.getButtonStatus('A') == BUTTON_PRESSED) {
           threshold += 10;
           ai.setMotinoThreshold(threshold);
       }
       
       // Botão B: diminuir threshold (-10)
       if (k10.getButtonStatus('B') == BUTTON_PRESSED) {
           threshold -= 10;
           ai.setMotinoThreshold(threshold);
       }
       
       // Display: mostrar threshold atual
   }
   ```

2. Fazer polichinelos em diferentes velocidades
3. Anotar threshold que detecta consistentemente
4. Atualizar `CALIBRATION.md` com resultados

### Fase 3: Validação de Contagem
**Objetivo:** Garantir que ciclos são contados corretamente

**Testes:**
1. Fazer 10 polichinelos lentos (1 por segundo)
2. Verificar se contador = 10 (±1)
3. Fazer 10 polichinelos rápidos (2 por segundo)
4. Verificar precisão

**Métricas de sucesso:**
- Precisão > 90% em velocidade normal (1 Hz)
- Falsos positivos < 5%
- Falsos negativos < 5%

### Fase 4: Validação Temporal
**Objetivo:** Confirmar filtros de tempo funcionam

**Testes:**
1. Movimentos muito rápidos (< 300ms) → NÃO devem contar
2. Movimentos muito lentos (> 3s) → NÃO devem contar
3. Movimentos normais (0.5-2s) → DEVEM contar

---

## 📊 Plano de Desenvolvimento Incremental

### Sprint 1: Setup e Validação (1-2 dias)
- [x] Pesquisar documentação K10
- [x] Configurar PlatformIO
- [x] Escrever este plano
- [x] Clonar repositório GitHub
- [x] Refatorar estrutura de diretórios
- [ ] Criar projeto no PlatformIO
- [ ] Upload de "Hello K10"
- [ ] Upload de MVP com LED

### Sprint 2: Detecção de Movimento (2-3 dias)
- [ ] Implementar lógica de detecção
- [ ] Calibrar threshold (testes de sensibilidade)
- [ ] Documentar calibração
- [ ] Commit: `feature/movement-detection`

### Sprint 3: Contador Básico (2-3 dias)
- [ ] Implementar máquina de estados
- [ ] Adicionar validação temporal
- [ ] Testar contagem com polichinelos
- [ ] Commit: `feature/counter-logic`

### Sprint 4: Interface de Usuário (1-2 dias)
- [ ] Melhorar display (título, contador grande)
- [ ] Adicionar controles com botões (reset, pause)
- [ ] Feedback sonoro (beep no contador)
- [ ] Commit: `feature/display-ui`

### Sprint 5: Refinamento e Testes (2-3 dias)
- [ ] Testes de precisão (100 repetições)
- [ ] Ajustes finos de threshold
- [ ] Documentar limitações
- [ ] Preparar README final

### Sprint 6: Release (1 dia)
- [ ] Merge para `main`
- [ ] Tag `v1.0.0`
- [ ] Criar release no GitHub
- [ ] Vídeo de demonstração

---

## ⚠️ Limitações Conhecidas

### Técnicas
1. **Detecção Binária:** Sem dados espaciais (pose estimation)
   - **Impacto:** Conta qualquer movimento, não valida se é polichinelo
   - **Mitigação:** Validação temporal (rejeitar movimentos muito rápidos/lentos)

2. **Dependência de Iluminação:** Câmera pode falhar em ambientes escuros
   - **Mitigação:** Adicionar aviso no README sobre iluminação mínima

3. **Ângulo da Câmera:** FOV 80° requer posicionamento correto
   - **Mitigação:** Documentar posicionamento ideal (1-2m de distância)

4. **FPS Desconhecido:** Não sabemos taxa de quadros da GC2145
   - **Mitigação:** Testar empiricamente com delay(50) no loop

### Funcionais
1. **Sem Diferenciação de Exercícios:** Conta qualquer movimento cíclico
2. **Sem Validação de Forma:** Não verifica se polichinelo está correto
3. **Contagem Aproximada:** Precisão estimada 85-95%

---

## 📚 Referências

### Documentação Oficial
- [K10 PlatformIO Setup](https://www.unihiker.com/wiki/K10/GettingStarted/gettingstarted_platformio/)
- [Platform Unihiker GitHub](https://github.com/DFRobot/platform-unihiker)
- [K10 Hardware Specs](https://www.unihiker.com/wiki/K10/)

### APIs
- `AIRecognition::Move` - Detecção de movimento binária
- `UNIHIKER_K10::canvas` - Display e canvas
- `UNIHIKER_K10::rgb` - Controle de LED RGB

### Decisões Técnicas
- **Por que não OpenCV?** ESP32-S3 não tem recursos para rodar modelos CNN
- **Por que não TensorFlow Lite?** K10 já tem `AIRecognition` nativo otimizado
- **Por que não acelerômetro?** Restrição do usuário (câmera apenas)

---

## 🎯 Próximos Passos Imediatos

1. ✅ **CONCLUÍDO:** Clonar repositório e refatorar estrutura

2. **AGORA:** Criar projeto PlatformIO
   ```bash
   # VSCode: Ctrl+Shift+P → PlatformIO: New Project
   # Nome: TinyGymBuddy
   # Board: Qualquer DFRobot → Modificar platformio.ini depois
   ```

3. **DEPOIS:** Modificar `platformio.ini` com config K10

4. **TESTAR:** Upload de "Hello K10" (código exemplo básico)

5. **VALIDAR:** LED RGB muda de cor com código MVP

6. **ITERAR:** Seguir sprints do plano de desenvolvimento

---

**Última atualização:** 01/01/2026  
**Status:** 🟡 Estrutura Refatorada - Aguardando Criação do Projeto PlatformIO  
**Próxima ação:** Criar projeto PlatformIO no VSCode
