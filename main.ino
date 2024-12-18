#include <WiFi.h>
#include <WebServer.h>
#include "arduinoFFT.h"
#include "SD.h"

// Pinos do cartão SD
#define SD_CS 15
#define SD_SCK 18
#define SD_MOSI 23
#define SD_MISO 19

#define SAMPLES 250              // Número de amostras
double vReal[SAMPLES];
double vImag[SAMPLES];
ArduinoFFT<double> fft(vReal, vImag, SAMPLES, 250); // Taxa padrão de amostragem

// Configurações Wi-Fi
const char* ssid = "mika";
const char* password = "mikamika";
WebServer server(80);

// Dados ECG
double ecgNormal[SAMPLES];
double samplingRate = 250; // Taxa de amostragem padrão
double gain = 200;         // Ganho padrão do sinal

// Função para ler o cabeçalho 100.hea
void readHeader() {
  File file = SD.open("/100.hea", FILE_READ);
  if (!file) {
    Serial.println("Erro ao abrir 100.hea!");
    return;
  }

  String line = file.readStringUntil('\n');
  sscanf(line.c_str(), "100 %*d %lf", &samplingRate); // Lê a taxa de amostragem
  Serial.print("Taxa de Amostragem: ");
  Serial.println(samplingRate);

  gain = 200.0; // O ganho geralmente vem fixo em 200 (ajustável se necessário)
  file.close();
}

// Função para ler o arquivo binário 100.dat
bool loadECGFromDat() {
  File file = SD.open("/100.dat", FILE_READ);
  if (!file) {
    Serial.println("Erro ao abrir 100.dat!");
    return false;
  }

  int index = 0;
  while (file.available() && index < SAMPLES) {
    int16_t value; // Lê os dados em 16 bits
    file.readBytes((char*)&value, sizeof(value));
    ecgNormal[index++] = value / gain; // Aplica o ganho
  }

  file.close();
  Serial.println("Dados do 100.dat carregados com sucesso!");
  return true;
}

String generateDashboard() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>ECG Monitor</title>";
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  html += "<style>body { background-color: black; color: limegreen; text-align: center; }";
  html += "h1 { color: white; } button { background-color: #333; color: white; padding: 10px; margin: 5px; border: none; }";
  html += "button:hover { background-color: limegreen; } canvas { background-color: black; border: 2px solid limegreen; }</style></head>";
  html += "<body><h1>ECG Monitor</h1>";
  html += "<button onclick=\"location.href='/fft'\">FFT</button>";
  html += "<button onclick=\"location.href='/normal'\">Normal</button>";
  html += "<button onclick=\"location.href='/laplace'\">Laplace</button>";
  html += "<button onclick=\"location.href='/state-space'\">Espaço de Estados</button>";
  html += "<canvas id='chart'></canvas>";
  html += "<script>function plotData(labels, data) {";
  html += "var ctx = document.getElementById('chart').getContext('2d');";
  html += "new Chart(ctx, {type: 'line', data: {labels: labels, datasets: [{label: 'ECG Signal', data: data, borderColor: 'limegreen', borderWidth: 2}]}});";
  html += "} </script></body></html>";
  return html;
}

void handleNormal() {
  String labels = "[";
  String data = "[";

  for (int i = 0; i < SAMPLES; i++) {
    labels += String(i) + (i < SAMPLES - 1 ? "," : "");
    data += String(ecgNormal[i], 4) + (i < SAMPLES - 1 ? "," : "");
  }
  labels += "]";
  data += "]";

  String response = generateDashboard();
  response += "<script>plotData(" + labels + "," + data + ");</script>";
  server.send(200, "text/html", response);
}

void handleFFT() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = ecgNormal[i];
    vImag[i] = 0;
  }

  fft.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  fft.compute(FFT_FORWARD);
  fft.complexToMagnitude();

  String labels = "[";
  String data = "[";
  for (int i = 0; i < SAMPLES / 2; i++) {
    double freq = (i * 1.0 / SAMPLES) * samplingRate;
    labels += String(freq, 2) + (i < SAMPLES / 2 - 1 ? "," : "");
    data += String(vReal[i], 2) + (i < SAMPLES / 2 - 1 ? "," : "");
  }
  labels += "]";
  data += "]";

  String response = generateDashboard();
  response += "<script>plotData(" + labels + "," + data + ");</script>";
  server.send(200, "text/html", response);
}

void handleLaplace() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = ecgNormal[i];
    vImag[i] = 0;
  }

  fft.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  fft.compute(FFT_FORWARD);
  fft.complexToMagnitude();

  String labels = "[";
  String data = "[";
  for (int i = 0; i < SAMPLES / 2; i++) {
    double freq = (i * 1.0 / SAMPLES) * samplingRate;
    labels += String(freq, 2) + (i < SAMPLES / 2 - 1 ? "," : "");
    data += String(vReal[i], 2) + (i < SAMPLES / 2 - 1 ? "," : "");
  }
  labels += "]";
  data += "]";

  String response = generateDashboard();
  response += "<script>plotData(" + labels + "," + data + ");</script>";
  server.send(200, "text/html", response);
}

void handleStateSpace() {
  // Matrizes de espaço de estados (exemplo simplificado)
  String A = "[[0, 1], [-1, -0.5]]";
  String B = "[[0], [1]]";
  String C = "[[1, 0]]";
  String D = "[[0]]";

  String response = generateDashboard();
  response += "<p>Matriz A: " + A + "</p>";
  response += "<p>Matriz B: " + B + "</p>";
  response += "<p>Matriz C: " + C + "</p>";
  response += "<p>Matriz D: " + D + "</p>";
  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);
  
  // Inicialização do cartão SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Falha ao inicializar o cartão SD!");
    while (1);
  }
  Serial.println("Cartão SD inicializado!");

  // Ler o cabeçalho e dados
  readHeader();
  if (!loadECGFromDat()) {
    Serial.println("Não foi possível carregar os dados do ECG.");
  }

  // Configuração Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.println(WiFi.localIP());

  // Configuração do servidor web
  server.on("/", []() { server.send(200, "text/html", generateDashboard()); });
  server.on("/fft", handleFFT);
  server.on("/normal", handleNormal);
  server.on("/laplace", handleLaplace);
  server.on("/state-space", handleStateSpace);
  server.begin();
  Serial.println("Servidor iniciado!");
}

void loop() {
  server.handleClient();
}
