# Artefato de Airsoft com Arduino

Este projeto é um artefato multifuncional para jogos de airsoft, desenvolvido com Arduino. Ele pode operar em três modos de jogo distintos: Explosão, Conquista e Dominação. Cada modo oferece uma experiência única para aumentar a imersão e a complexidade das partidas.

## Modos de Jogo

### 1. Explosão
Simula a desativação de um artefato onde uma equipe deve armar e outra desarmar dentro de um tempo determinado.

**Funcionamento:**
- **Configuração do Tempo:** Ajustável através do menu.
- **Armar o Artefato:** Pressionar e segurar o botão por 7 segundos.
- **Desarmar o Artefato:** Pressionar e segurar o botão por 10 segundos.

### 2. Conquista
Os jogadores devem capturar e manter uma posição por um período de tempo.

**Funcionamento:**
- **Configuração do Tempo:** Ajustável através do menu.
- **Domínio da Posição:** Pressionar e segurar o botão até o tempo configurado.
- **Mudança de Controle:** Outra equipe pode capturar a posição pressionando o botão correspondente.

### 3. Dominação
Similar ao modo Conquista, mas com registro do tempo de domínio de cada equipe. A equipe que mantiver a posição por mais tempo vence.

**Funcionamento:**
- **Configuração do Tempo:** Ajustável através do menu.
- **Domínio da Posição:** Registro do tempo de domínio de cada equipe.
- **Determinação do Vencedor:** A equipe com mais tempo de domínio vence ao final do jogo.

## Componentes Utilizados

- **Arduino Uno:** Placa principal do projeto.
- **Display LCD 16x2:** Exibe informações como tempo restante e status do artefato.
- **Botões:** Permitem interação com o artefato.
- **Relé:** Simula a ativação do artefato.
- **Resistores:** Protegem os componentes.

## Montagem do Circuito

### Esquema de Ligação
1. Conecte os pinos do display LCD ao Arduino conforme o esquema.
2. Conecte os botões de controle nos pinos digitais do Arduino.
5. Conecte o relé para simular a ativação do artefato.

### Código Fonte
O código completo do projeto está disponível neste repositório. Certifique-se de ter a IDE do Arduino instalada e as bibliotecas necessárias configuradas.

## Desafios do Desenvolvimento

- **Gerenciamento de Tempo:** Implementar contagem regressiva precisa.
- **Interface de Usuário:** Criar uma interface intuitiva com display LCD e botões.
- **Controle de Estado:** Gerenciar diferentes estados do jogo.

## Resultados e Aprendizados

Este projeto proporcionou uma excelente oportunidade para aplicar conhecimentos de eletrônica e programação. A modularização do código facilitou a implementação dos diferentes modos de jogo.

## Futuras Melhorias

- **Conectividade Sem Fio:** Para configuração e monitoramento remoto.
- **Mais Modos de Jogo:** Adicionar novos modos para variedade.
- **Integração com Sensores:** Adicionar novos desafios e dinamismo ao jogo.

## Documentação e Suporte

Toda a documentação, incluindo esquemas de circuitos, lista de componentes e instruções de montagem, está disponível neste repositório. Estamos disponíveis para suporte através da plataforma.

## Conclusão

Este projeto combina eletrônica, programação e airsoft para criar uma experiência de jogo mais realista e envolvente. Esperamos inspirar outros a inovar e desenvolver suas próprias soluções tecnológicas para o airsoft.

## Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## Contato

Para dúvidas e sugestões, sinta-se à vontade para abrir uma issue ou enviar um pull request. Agradecemos seu interesse e colaboração!
