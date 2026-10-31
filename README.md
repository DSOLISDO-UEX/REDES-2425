# REDES-2425

Prácticas de Fundamentos de Redes de Comunicaciones (curso 2024/25).

Este repositorio contiene una implementación en C++ de varios ejercicios y servicios de comunicación sobre una capa de enlace proporcionada por la práctica. El objetivo es trabajar con tramas Ethernet-like, gestión de interfaces, descubrimiento de estaciones y protocolos de nivel de aplicación orientados a comunicación entre estaciones maestras y esclavas.

## Índice

- [Descripción general](#descripción-general)
- [Características principales](#características-principales)
- [Estructura del repositorio](#estructura-del-repositorio)
- [Requisitos](#requisitos)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Scripts de prueba](#scripts-de-prueba)
- [Autores](#autores)

## Descripción general

El proyecto se centra en la creación y manejo de comunicaciones entre estaciones sobre interfaces de red virtuales. Se usa una librería auxiliar `linkLayer` junto con clases dedicadas a encapsular la lógica de:

- apertura y configuración de adaptadores de red,
- construcción de tramas,
- envío/recepción de paquetes,
- gestión de mecanismos de descubrimiento,
- servicios de comunicación específicos para distintas sesiones prácticas.

El código está organizado como una pequeña arquitectura modular, con clases que encapsulan cada parte del comportamiento del sistema y con varios archivos de sesión que sirven como puntos de entrada para cada ejercicio.

## Características principales

- Gestión de interfaces de red con `Adapter`.
- Construcción de paquetes/tramas con `FrameBuilder`.
- Lógica de comunicación compartida con `Communication`.
- Modelado de estaciones con `GenericStation`, `MasterStation` y `SlaveStation`.
- Soporte para servicios interactivos y protocolos de espera/paro (`WaitAndStop`).
- Ejercicios prácticos organizados por sesiones:
  - `Sesion1.cpp`
  - `Sesion2.cpp`
  - `Sesion3.cpp`
  - `Sesion4.cpp`
- Integración con `libpcap` y multihilo (`pthread`).
- Compatibilidad con entornos Linux y redes virtuales (`veth`, namespaces de red).

## Estructura del repositorio

```text
REDES-2425/
├── .clang-format              # Configuración de formato ClangFormat
├── .gitignore                 # Ignorar archivos de compilación y temporales
├── .vscode/
│   ├── launch.json            # Configuración de depuración VS Code
│   └── tasks.json             # Tarea de compilación para g++
├── SCRIPTS/
│   └── FRC_enlace.sh          # Script para crear interfaces de red de prueba
├── build/                     # Directorio de salida de compilación
├── include/                   # Cabeceras del proyecto
│   ├── Adapter.h
│   ├── Communication.h
│   ├── FrameBuilder.h
│   ├── GenericStation.h
│   ├── MasterStation.h
│   ├── SlaveStation.h
│   ├── linkLayer.h
│   ├── kbhit.h
│   ├── lista.h
│   ├── Interactive/
│   └── WaitAndStop/
├── src/                       # Código fuente principal
│   ├── Adapter.cpp
│   ├── Communication.cpp
│   ├── FrameBuilder.cpp
│   ├── GenericStation.cpp
│   ├── MasterStation.cpp
│   ├── SlaveStation.cpp
│   ├── Sesion1.cpp
│   ├── Sesion2.cpp
│   ├── Sesion3.cpp
│   ├── Sesion4.cpp
│   ├── Interactive/
│   ├── WaitAndStop/
│   └── linkLayer.a
├── Autores.md                 # Información de autores y grupo
├── Consideraciones.md         # Descripción del diseño del proyecto
├── EProtoc.txt                # Ejemplo/protocolo de referencia
├── envio.txt                  # Archivo de mensajes / ejemplo de tráfico
├── README.md                  # Documentación del proyecto
└── .
