FROM gcc:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    libssl-dev \
    tcl \
    xclip \
    && apt-get clean

WORKDIR /app

RUN wget https://github.com/sqlcipher/sqlcipher/archive/refs/tags/v4.5.0.tar.gz && \
    tar xzf v4.5.0.tar.gz && \
    cd sqlcipher-4.5.0 && \
    ./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC" LDFLAGS="-lcrypto" && \
    make && make install && \
    echo "/usr/local/lib" > /etc/ld.so.conf.d/sqlcipher.conf && ldconfig

COPY ./src ./src
COPY ./database ./database

WORKDIR /app/src
RUN g++ -std=c++17 \
    main.cpp db_creation.cpp db_modification.cpp db_view.cpp utils.cpp \
    -Iinclude \
    -lsqlcipher -lcrypto -lpthread \
    -o /app/password_storage_system

WORKDIR /app
CMD ["./password_storage_system"]
