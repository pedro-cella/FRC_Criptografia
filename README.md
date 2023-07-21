# Labortatório sobre criptografia simétrica e assimétrica

## Fractal

![FRACTAL](./fractaljulia.bmp)

### Observações

#### 1 - Compile usando

```bash
gcc fractal.c -o fractal.bin -lm
```

#### 2 - Rode o programa com

```bash
./fractal.bin 153
```

Para que seja criado um fractal para poder ser utilizado durante as partes do projeto DES e RSA.

## DES

Criptografia simetrica, utilizamos uma PSK para poder fazer a encriptação do fractal gerado.

Utilizamos uma conexão TCP/IP para que seja possivel fazer a transferencia do arquivo encriptado.

Aqui está uma visão geral simplificada do processo de encriptação DES:

1. Chave de 56 bits: O algoritmo utiliza uma chave de 56 bits para a encriptação. Os outros 8 bits são usados como paridade e não têm efeito na encriptação em si.

2. Geração das subchaves: A chave de 56 bits é transformada em 16 subchaves de 48 bits cada uma. Essas subchaves são geradas por meio de permutações e deslocamentos.

3. Divisão em blocos: A mensagem de entrada é dividida em blocos de 64 bits.

4. Inicialização: Cada bloco é permutado inicialmente antes de ser processado pelas rodadas.

5. Rodadas (Feistel Network): O bloco de 64 bits é processado em uma série de 16 rodadas idênticas. Cada rodada aplica uma função complexa que envolve substituições, permutações e operações lógicas com as subchaves geradas anteriormente.

6. Permutação final: Após todas as rodadas, é aplicada uma última permutação para reorganizar os bits finais do bloco.

7. Bloco encriptado: O resultado final da encriptação é um bloco de 64 bits transformado.

### Observações

**Verifique se tem um arquivo fractaljulia.bmp na raiz do projeto.**

#### 1 - Compile usando

```bash
1. gcc server.c des.c -O3 -o server.bin -lm -lcrypto 
2. gcc client.c des.c -O3 -o client.bin -lm -lcrypto
```

### Instruções de Uso

#### Configurações

Para poder definir em qual porta o servidor vai rodar.

**server.c**

![Server port](Assets/server_port.png)

**client.c**
![Client config](Assets/client_config.png)

Configurações para poder dizer onde que o servidor está rodando.

#### 1 - Inicie o servidor

Para que seja possivel o client fazer a troca de arquivos e chave.

```bash
    ./server.bin
```

#### 2 - Inicie o client

```bash
    ./client.bin ../fractaljulia.bmp
```


## RSA

sudo apt-get install libssl-dev

gcc gerarsa.c -lssl -lcrypto -o gerarsa.bin -O3 && ./gerarsa.bin -p

gcc gerarsa.c -lssl -lcrypto -o gerarsa.bin -O3 && ./gerarsa.bin -k ./primos.txt

gcc rsa_encrypt.c -lssl -lcrypto -o encrypt.bin -O3 && ./encrypt.bin ../fractaljulia.bmp

gcc rsa_decrypt.c -lssl -lcrypto -o decrypt.bin -O3 && ./decrypt.bin