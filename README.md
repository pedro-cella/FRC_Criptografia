# Labortatório sobre criptografia simétrica e assimétrica

## Fractal

![FRACTAL](./fractaljulia.bmp)

### Observações

#### 2- Compile usando

```bash
gcc fractal.c -o fractal -lm
```

## DES

### Observações

#### 1- Verifique se os arquivos des.c, des.h and run_des.c estão no mesmo diretório

#### 2- Compile usando

    gcc -O3 des.c run_des.c -o run_des.o

### Instruções de Uso

#### 1- Gere uma keyfile com:
    ./run_des.o -g /tmp/keyfile.key    

#### 2- Encripte o arquivo bmp usando:
    ./run_des.o -e keyfile.key arquivo.bmp arquivo.enc

#### 3- Desecripte o arquivo usando:
    ./run_des.o -d keyfile.key arquivo.enc arquivo_decrypted.bmp

gcc client.c des.c -O3 -o client.bin -lm -lcrypto && ./client.bin ../fractaljulia.bmp

gcc server.c des.c -O3 -o server.bin -lm -lcrypto && ./server.bin


gcc gerarsa.c -lssl -lcrypto -o gerarsa.bin -O3 && ./gerarsa.bin -p

gcc gerarsa.c -lssl -lcrypto -o gerarsa.bin -O3 && ./gerarsa.bin -k ./primos.txt

gcc rsa_encrypt.c -lssl -lcrypto -o encrypt.bin -O3 && ./encrypt.bin ../fractaljulia.bmp

gcc rsa_decrypt.c -lssl -lcrypto -o decrypt.bin -O3 && ./decrypt.bin
## RSA