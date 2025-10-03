# Assignment: Systemd Service que registra un sensor simulado

Este proyecto implementa un **servicio en systemd** que arranca en `multi-user.target` y ejecuta un programa en **C** que simula la lectura de un sensor.  
El servicio registra periódicamente un valor junto con la **marca de tiempo ISO-8601** en un archivo de log bajo `/tmp`.  
Si `/tmp` no está disponible, utiliza `/var/tmp` como ruta de respaldo.

---

## 📌 Características

- Programa escrito en **C**, compilado con `make`.
- Registro en archivo: `TIMESTAMP | VALUE`.
- Intervalo configurable (por defecto: 5 segundos).
- Manejo de **SIGTERM** para apagado limpio.
- Arranca automáticamente con **systemd** en `multi-user.target`.

---

## 📂 Estructura del proyecto

├─ src/ # código fuente en C
│ └─ main.c
├─ systemd/
│ └─ assignment-sensor.service
├─ ai/ # evidencia de uso de IA
│ ├─ prompt-log.md
│ ├─ reflection.md
│ └─ provenance.json
├─ tests/
│ └─ manual-tests.md
├─ Makefile
└─ README.md

---

## ⚙️ Requisitos

- Linux con `systemd` (ej. Debian, Ubuntu, Kali, etc.).
- Compilador de C (`gcc`).
- Acceso `sudo` para instalar el servicio.

---

## 🚀 Instrucciones de uso

### 1. Clonar y compilar

```bash
git clone <URL_DE_TU_REPO>
cd assignment-sensor
make
```

### 2. Instalar binario y servicio

```bash
sudo cp assignment-sensor /usr/local/bin/
sudo cp systemd/assignment-sensor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service
```

### 3. Verificar estado

```bash
systemctl status assignment-sensor
```

### 4. Ver logs

```bash
tail -f /tmp/assignment_sensor.log
```

Si /tmp no es escribible, los logs se guardarán en:

```bash
/var/tmp/assignment_sensor.log
```

### 5. Detener el servicio

```bash
sudo systemctl stop assignment-sensor
```

El programa recibe SIGTERM, cierra el archivo y termina correctamente.

### 6. Desinstalar

```bash
sudo systemctl disable --now assignment-sensor.service
sudo rm /usr/local/bin/assignment-sensor
sudo rm /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
```
