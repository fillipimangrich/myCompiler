# Trabalho Final da Disciplina de Compiladores

## Instruções para executar

O professor pode compilar o projeto da seguinte forma:

```bash
make build
```

Ou então, mais praticamente, podem ser utilizados os comandos a seguir para compilar e rodar os testes específicos:

```bash
make test1 # Roda o primeiro teste
make test2 # Roda o segundo teste
make test3 # Roda o terceiro teste
```
A partir disso, o projeto pode ser limpo com um `make clean`.

O compilador deve então mostrar um feedback de erros e avisos, caso ocorra algum, ou então deve gerar um arquivo de código intermediário no formato de TAC - Código de Três Endereços.

O arquivo gerado será encontrado na pasta base do projeto, com a extensão `.tac`.
