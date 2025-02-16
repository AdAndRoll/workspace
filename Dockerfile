FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y build-essential gdb cmake git libssl-dev openssl curl \
                       net-tools iproute2  # Добавляем утилиты

WORKDIR /workspace

EXPOSE 8080

CMD ["bash"]
