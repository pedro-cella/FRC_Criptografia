# Labortatório sobre criptografia simétrica e assimétrica

## DES

### Observações

#### 1- Verifique se os arquivos des.c, des.h and run_des.c estão no mesmo diretório

#### 2- Compile usando: 

    gcc -O3 des.c run_des.c -o run_des.o

### Instruções de Uso

#### 1- Gere uma keyfile com:
    ./run_des.o -g /tmp/keyfile.key    

#### 2- Encripte o arquivo bmp usando:
    ./run_des.o -e keyfile.key arquivo.bmp arquivo.enc

#### 3- Desecripte o arquivo usando:
    ./run_des.o -d keyfile.key arquivo.enc arquivo_decrypted.bmp


## RSA