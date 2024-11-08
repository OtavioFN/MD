#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long long maximo_divisor_comum(unsigned long long valor1, unsigned long long valor2) {
    while (valor2 != 0) {
        unsigned long long valor_temporario = valor2;
        valor2 = valor1 % valor2;
        valor1 = valor_temporario;
    }
    return valor1;
}

unsigned long long exponenciacao_modular(unsigned long long base, unsigned long long expoente, unsigned long long modulo) {
    unsigned long long resultado = 1;
    base = base % modulo;
    while (expoente > 0) {
        if (expoente % 2 == 1)
            resultado = (resultado * base) % modulo;
        expoente = expoente >> 1;
        base = (base * base) % modulo;
    }
    return resultado;
}

long long calcular_inverso_modular(long long expoente, long long phi) {
    long long valor_t = 0, novo_valor_t = 1;
    long long resto = phi, novo_resto = expoente;
    while (novo_resto != 0) {
        long long quociente = resto / novo_resto;
        long long valor_temporario = novo_valor_t;
        novo_valor_t = valor_t - quociente * novo_valor_t;
        valor_t = valor_temporario;

        valor_temporario = novo_resto;
        novo_resto = resto - quociente * novo_resto;
        resto = valor_temporario;
    }
    if (resto > 1) return -1; // Sem inverso modular
    if (valor_t < 0) valor_t += phi;
    return valor_t;
}

// Função para gerar chaves públicas e privadas
void gerar_chave_publica(unsigned long long primo_p, unsigned long long primo_q, unsigned long long expoente) {
    unsigned long long modulo_n = primo_p * primo_q;
    unsigned long long phi = (primo_p - 1) * (primo_q - 1);

    if (maximo_divisor_comum(expoente, phi) != 1) {
        printf("Erro: 'e' não é relativamente primo a φ(n).\n");
        return;
    }

    long long inverso_privado_d = calcular_inverso_modular(expoente, phi);
    if (inverso_privado_d == -1) {
        printf("Erro ao calcular o inverso modular.\n");
        return;
    }

    FILE *arquivo_publico = fopen("public_key.txt", "w");
    if (arquivo_publico) {
        fprintf(arquivo_publico, "e: %llu\nn: %llu\n", expoente, modulo_n);
        fclose(arquivo_publico);
        printf("Chave pública salva em 'public_key.txt'.\n");
    } else {
        printf("Erro ao salvar o arquivo.\n");
    }

    FILE *arquivo_privado = fopen("private_key.txt", "w");
    if (arquivo_privado) {
        fprintf(arquivo_privado, "d: %lld\nn: %llu\n", inverso_privado_d, modulo_n);
        fclose(arquivo_privado);
        printf("Chave privada salva em 'private_key.txt'.\n");
    } else {
        printf("Erro ao salvar o arquivo.\n");
    }
}

// Função para encriptar uma mensagem
void encriptar_mensagem(const char *mensagem, unsigned long long expoente, unsigned long long modulo_n) {
    FILE *arquivo_encriptado = fopen("encrypted_message.txt", "w");
    if (!arquivo_encriptado) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; mensagem[i] != '\0'; i++) {
        if (mensagem[i] < 32 || mensagem[i] > 126) {
            printf("Erro: Caracteres fora do intervalo permitido (32 a 126).\n");
            fclose(arquivo_encriptado);
            return;
        }

        unsigned long long mensagem_encriptada = exponenciacao_modular((unsigned long long)mensagem[i], expoente, modulo_n);
        fprintf(arquivo_encriptado, "%llu ", mensagem_encriptada);
    }
    fclose(arquivo_encriptado);
    printf("Mensagem encriptada salva em 'encrypted_message.txt'.\n");
}

// Função para desencriptar uma mensagem
void desencriptar_mensagem(const char *nome_arquivo, unsigned long long inverso_privado_d, unsigned long long modulo_n) {
    FILE *arquivo_encriptado = fopen(nome_arquivo, "r");
    if (!arquivo_encriptado) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    FILE *arquivo_desencriptado = fopen("decrypted_message.txt", "w");
    if (!arquivo_desencriptado) {
        printf("Erro ao criar o arquivo de saída.\n");
        fclose(arquivo_encriptado);
        return;
    }

    unsigned long long mensagem_encriptada;
    while (fscanf(arquivo_encriptado, "%llu", &mensagem_encriptada) != EOF) {
        unsigned long long mensagem_desencriptada = exponenciacao_modular(mensagem_encriptada, inverso_privado_d, modulo_n);
        fprintf(arquivo_desencriptado, "%c", (char)mensagem_desencriptada);
    }

    fclose(arquivo_encriptado);
    fclose(arquivo_desencriptado);
    printf("Mensagem desencriptada salva em 'decrypted_message.txt'.\n");
}

int main() {
    unsigned long long primo_p, primo_q, expoente;
    printf("Digite p (primo com pelo menos 6 dígitos): ");
    scanf("%llu", &primo_p);
    printf("Digite q (primo com pelo menos 6 dígitos): ");
    scanf("%llu", &primo_q);
    printf("Digite e (relativamente primo a φ(n)): ");
    scanf("%llu", &expoente);

    gerar_chave_publica(primo_p, primo_q, expoente);

    char mensagem[256];
    printf("Digite a mensagem a ser encriptada: ");
    scanf(" %[^\n]s", mensagem);
    unsigned long long modulo_n = primo_p * primo_q;

    encriptar_mensagem(mensagem, expoente, modulo_n);

    long long inverso_privado_d = calcular_inverso_modular(expoente, (primo_p - 1) * (primo_q - 1));
    desencriptar_mensagem("encrypted_message.txt", inverso_privado_d, modulo_n);

    return 0;
}
