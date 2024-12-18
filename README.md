Este código implementa um sistema de monitoramento de ECG (eletrocardiograma) usando uma ESP32. Ele usa a biblioteca WiFi para conexão sem fio, a biblioteca WebServer para criar uma interface web, a biblioteca arduinoFFT para realizar a Transformada Rápida de Fourier (FFT), e a biblioteca SD para ler dados de um cartão SD, e dá a opção de disponibilidade de visualizar os dados por laplace, e visualizar o espaço de estados pelo servidor web(dashboard).

---

## **O que o código faz?**

### **1. Funcionalidades principais:**
- **Conexão Wi-Fi**: Conecta a ESP32 a uma rede Wi-Fi.
- **Servidor Web**: Cria uma página web que permite visualizar dados ECG e suas análises (FFT, espaço de estados, etc.).
- **Leitura de Arquivos SD**:
  - Lê o cabeçalho (`100.hea`) para determinar a taxa de amostragem.
  - Lê os dados binários (`100.dat`) e aplica ganho aos sinais ECG.
- **Análise do sinal ECG**:
  - Exibição do sinal original.
  - Análise de frequência usando FFT.
  - Apresentação de modelos em espaço de estados (simplificado no código).
- **Interface Web**: Inclui botões para alternar entre análises e um gráfico interativo baseado na biblioteca Chart.js.

---

### **Como rodar o código?**

#### **1. Hardware necessário:**
- **ESP32**.
- **Cartão SD** com os arquivos:
  - `100.hea` (arquivo de cabeçalho com informações do ECG).
  - `100.dat` (arquivo binário com os dados do ECG).
- Conexões adequadas:
  - **Pinos do cartão SD**: Conecte aos pinos especificados no código (`SD_CS`, `SD_SCK`, etc.).
  - **Alimentação**: Certifique-se de que a ESP32 esteja alimentada corretamente.

---

#### **2. Configuração do software:**

1. **Bibliotecas necessárias**:
   - Certifique-se de que as bibliotecas abaixo estejam instaladas no Arduino IDE:
     - `WiFi.h`
     - `WebServer.h`
     - `arduinoFFT.h`
     - `SD.h`

2. **Configuração Wi-Fi**:
   - Altere as linhas abaixo com o nome e senha da sua rede Wi-Fi:
     ```cpp
     const char* ssid = "mika";       // Nome da rede
     const char* password = "mikamika"; // Senha da rede
     ```

3. **Configuração do cartão SD**:
   - Verifique se os pinos conectados ao cartão SD estão corretos.

4. **Carregar o código na ESP32**:
   - Compile e faça o upload do código para a ESP32 usando o Arduino IDE.

---

#### **3. Acessando o sistema:**
1. Após o upload, abra o monitor serial para verificar o progresso:
   - Mensagens como "Conectando ao Wi-Fi..." e "Servidor iniciado!" devem aparecer.
   - O IP local da ESP32 será exibido (ex.: `192.168.0.101`).
2. Acesse o endereço IP pelo navegador de qualquer dispositivo conectado à mesma rede Wi-Fi.
3. Use a interface web para:
   - Visualizar o sinal ECG normal.
   - Realizar análises FFT, Laplace ou em espaço de estados.

---

#### **Exemplo de saída no navegador:**
- Página com botões:
  - **Normal**: Mostra o sinal ECG original.
  - **FFT**: Mostra a análise de frequência.
  - **Laplace e Espaço de Estados**: Mostram análises relacionadas ao modelo matemático.
- Gráfico interativo exibindo o sinal ECG ou os dados analisados.

