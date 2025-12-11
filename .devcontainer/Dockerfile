# # Используем базовый образ с GCC и CMake
FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    libyaml-cpp-dev \
    && rm -rf /var/lib/apt/lists/*

# Копирование исходного кода
WORKDIR /app
COPY . .

# Сборка проекта
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# Установка entrypoint
ENTRYPOINT ["./build/dag_executor"]
