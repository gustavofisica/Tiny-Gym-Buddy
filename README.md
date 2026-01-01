# Tiny-Gym-Buddy 🏋️‍♂️

Contador de exercícios inteligente usando detecção de movimento por câmera na **Unihiker K10** (ESP32-S3).

## 📌 Sobre o Projeto

Sistema de contagem automática de repetições de exercícios (polichinelos) utilizando apenas a câmera integrada da Unihiker K10. Implementado em **Arduino C++** com **PlatformIO**, usando a biblioteca nativa `AIRecognition` para detecção de movimento.

### ✨ Características
- 📹 Detecção por câmera apenas (sem acelerômetro)
- 🧠 IA nativa do ESP32-S3 (AIRecognition)
- 🖥️ Display LCD 2.8" para feedback visual
- 💡 LED RGB para status de movimento
- ⚡ Validação temporal para evitar falsos positivos

### 🛠️ Tecnologias
- **Hardware:** Unihiker K10 (ESP32-S3, GC2145 2MP camera)
- **Framework:** Arduino C++ + PlatformIO
- **IDE:** VSCode com extensão PlatformIO
- **Bibliotecas:** `unihiker_k10.h`, `AIRecognition.h`

## 🚀 Como Usar

### Pré-requisitos
- VSCode instalado
- Extensão PlatformIO IDE
- Unihiker K10 conectada via USB

### Setup do Projeto
```bash
# Clonar repositório
git clone https://github.com/gustavofisica/Tiny-Gym-Buddy.git
cd Tiny-Gym-Buddy

# Abrir no VSCode
code .
```

### Compilar e Upload
1. Abrir projeto no VSCode
2. Aguardar download do SDK (primeira vez)
3. Compilar: `Ctrl+Alt+B`
4. Upload para K10: `Ctrl+Alt+U`

## 📖 Documentação

- [📋 Plano de Implementação](IMPLEMENTATION_PLAN.md) - Arquitetura completa do projeto
- [🔧 API Reference](docs/API_REFERENCE.md) - Referência das APIs K10 *(em breve)*
- [🎯 Calibration Guide](docs/CALIBRATION.md) - Guia de calibração de threshold *(em breve)*

## 🏗️ Estrutura do Projeto

```
Tiny-Gym-Buddy/
├── src/              # Código-fonte principal
├── lib/              # Bibliotecas customizadas
├── test/             # Testes unitários
├── docs/             # Documentação adicional
└── platformio.ini    # Configuração PlatformIO
```

## 📊 Status do Desenvolvimento

### Sprint 1: Setup e Validação ✅
- [x] Pesquisar documentação K10
- [x] Configurar estrutura do projeto
- [x] Escrever plano de implementação
- [ ] Criar projeto PlatformIO
- [ ] Testar "Hello K10"

### Próximas Etapas
- [ ] Implementar detecção de movimento básica
- [ ] Calibrar threshold de sensibilidade
- [ ] Desenvolver lógica de contagem
- [ ] Adicionar interface de usuário

## ⚠️ Limitações

- Detecção **binária** de movimento (sem pose estimation)
- Requer **iluminação adequada** para funcionamento da câmera
- Conta **qualquer movimento cíclico**, não valida forma do exercício
- Precisão estimada: **85-95%**

## 🤝 Contribuindo

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/MinhaFeature`)
3. Commit suas mudanças (`git commit -m 'feat: Adiciona MinhaFeature'`)
4. Push para a branch (`git push origin feature/MinhaFeature`)
5. Abra um Pull Request

## 📝 Licença

Este projeto é de código aberto para fins educacionais.

## 👤 Autor

**Gustavo**
- GitHub: [@gustavofisica](https://github.com/gustavofisica)

---

**Última atualização:** 01/01/2026  
**Versão:** 0.1.0-alpha (em desenvolvimento)
