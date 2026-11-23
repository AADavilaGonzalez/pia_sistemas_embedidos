# Sistema de sincronización para máquinas de enfriamiento

Proyecto de la materia **Sistemas Embedidos** de la **Facultad de Ciencias Físico-Matemáticas, Universidad Autónoma de Nuevo León**.

## Descripción

El proyecto implementa un par de dispositivos gemelos basados en **ESP32-S3** para ayudar a dos operadores a sincronizar la potencia de dos máquinas de enfriamiento utilizadas en conjunto. Los dispositivos intercambian información mediante **ESP-NOW** y utilizan LEDs para indicar:

- Estado de encendido de la máquina remota.
- Necesidad de aumentar la potencia local.
- Necesidad de disminuir la potencia local.
- Desincronización prolongada y estado de error.

La potencia se obtiene mediante una entrada analógica y el estado de cada máquina se controla mediante botones. El sistema considera una diferencia máxima de 200 unidades y activa el error cuando la desincronización se mantiene durante 3 segundos.

## Integrantes

- **Adrián Rivera Luna** — 2131513
- **Emilio Hernández Madrid** — 2086253
- **Aldo Adrián Dávila González** — 1994122

## Docente

**Gerardo Garza Garza**

## Archivos principales

- `programa.ino`: código principal del dispositivo.
- `mac.ino`: configuración relacionada con las direcciones MAC.
- `diagrama.drawio`: diagrama electrónico editable.
- `diagrama.png`: imagen del diagrama electrónico.

## Uso

1. Configurar en `programa.ino` la dirección MAC del ESP32-S3 remoto.
2. Conectar los LEDs, botones y el potenciómetro conforme al diagrama electrónico.
3. Cargar el programa en ambos ESP32-S3.
4. Ajustar manualmente la potencia de las máquinas siguiendo los LEDs de aumento y disminución.
