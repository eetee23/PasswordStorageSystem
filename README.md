# How to run docker file inside windows computer
docker build -t password_storage_system .
docker run -it -v "$(pwd -W)/database:/app/database" password_storage_system 
or
docker run -it -v "${PWD}/database:/app/database" password_storage_system