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

COPY ./main.cpp .
COPY ./database .

RUN g++ main.cpp -o PasswordGenerator -lsqlcipher -lcrypto -lpthread

CMD ["./PasswordGenerator"]
