# trab2-paralela

Ci1316 2sem2024  - trabalho 2 - versao 1.0
---------------...

Enunciado: 18/nov/24
DATA da Entrega: 28/nov/24  (são 11 dias)
Local de entrega: UFPR Virtual
Equipe de alunos:
   Esse trabalho pode ser feito em grupo de NO MAXIMO dois alunos(as)

----------------------    
Historico:
versao 1.0 versao inicial

OBS: essa especificacao pode receber pequenas alteracoes em breve
     para melhor clareza das atividades      
--------------------------

Nesse trabalho 2 vamos paralelizar a função abaixo,
também em Phreads, 
conforme essa especificacao inicial.

- As as funções devem ENCAPSULAR tudo
  para a paralelização 
  (conforme exemplo visto em aula para a redução).
  
  OU SEJA: voce DEVE basear suas soluçoes para as funcoes
           abaixo tomando como base 
           a solucao disponibilizada pelo prof. de reducao paralela 
           com POOL de threads
           - Soluções ENTREGUES SEM pool de threads terão nota bastante REDUZIDA.

- As suas novas solucoes devem ser feitas TAMBEM com pool de threads.

- (PLANILHAS) 
  Voce DEVE fazer as experiencias se baseando nas planilhas feitas pelo prof.
  (experiencias baseadas na planilha de reducao paralela com pool de threads)
  adaptando suas experiencias para os casos desse novo problema.
  BASEADO na planilha quer dizer, voce  deve entregar O MESMO TIPO DE PLANILHA
  adaptando formulas, graficos etc ADEQUADAMENTE para o novo problema.
  - A sua planilha de medicoes DEVE ser entregue.
    O prof. vai tentar reproduzir seus resultados com suas planilhas.
    Planilhas em formatos diferentes (aleatorios) nao serao aceitas.
    Simplesmente entregar tabelas em PDF na vale!
    - Portanto seus resultados deve ser consistentes e em formato similar
      na planilha
      de modo que o prof. possa reproduzir E CONFERIR seus resultados
    - voce pode copiar tabelas e graficos da sua planilha para seu relatorio
      para que o mesmo fique completo, mas tem de entregar planilha e relatorio
      e o fonte, e os scripts para rodar (sendo os scripts adaptados ou nao adaptados
      o prof precisa dos scripts e tudo o que for necessario para reproduzir e conferir, 
      CASO CONTRARIO o trabalho terá nota bastante reduzida)  

- EXPERIMENTOS nas planilhas e scripts
  Os scripts slurm devem rodar o programa 10 vezes
  DEVEMOS poder importar (copiar e colar) os resultados
  em uma das folhas da planilha (a folha de resultados)
  e a planilha DEVE calcular CORRETAMENTE as medias de tempo,
  vazao com as medidas corretas e fazer graficos.
  Voce DEVE ajustar as escalas para os experimentos
  especificados.
  Escalas NAO ajustadas ou calculos errados na planilha
  geram perdas de pontos no trabalho.       
      
- Para as experiencias:
  Somente serao aceitos resultados obtidos 
  na maquinas do cluster w (xeon), da disciplina, 
  executados com slurm (sbatch) em modo --exclusive
  o script para rodar seu trabalho em slurm DEVE ser entregue
  voce DEVE se basear no script slurm fornecido pelo professor
  (o que foi dado pelo professor para rodar a reducao paralela com o slurm)
  
   
  A quantidade de elementos para as experiencias:
  Vamos especificar aqui em breve.
  Tipicamente teremos:
  - Vetor Input de 16Milhoes de elementos long long
    (OBS: 16 Milhoes significa: 16000000)
    
  - Vetor de particionamento P também de LONG LONG, 
     podemos pensar em DUAS experiencias: 
     experiencia A: com 1000 (mil) elementos nesse vetor P
     experiencia B: com 100mil elementos nesse vetor P

  - Fazer os graficos e tabelas nas planilhas que DEVENDO medir variando
    o numero de threads  1, 2, 3, ... até 8 threads  
      (ATENCAO: fazer até 8 threads, mesmo que a maquina usada nao tenha tantos cores)
           
     
  - Como vamos gerar os vetores ?
    Vamos gerar numeros aleatorios para colocar nesses vetores
    os numeros aleatorios long long deve ser gerados chamando a funcao abaixo:
    
    long long geraAleatorioLL() {
      int a = rand();  // Returns a pseudo-random integer
	               //    between 0 and RAND_MAX.
      int b = rand();  // same as above
      long long v = (long long)a * 100 + b;
      return v;
    }  
	
    * O vetor de entrada Input NÂO é ordenado. É aleatório.
    * O vetor P de particionamentos, deve ser gerado assim:
      1) preencher o vetor P com np valores aleatórios 
           usando a funcao geraAleatorioLL()
      2) após preencher o vetor P de aleatórios ele deve ser ordenado.
         para ordenar esse vetor (no inicio do programa)
         voce deve chamar uma funcao de ordenaçao,
         Esss ordenaçao NAO precisa ser paralela.
    * A geração dos vetores Input e P tomam tempo de processamento
      MAS NAO DEVEMOS incluir isso nas medidas de tempo,
      ou seja, vamos manter a geracao dos vetores FORA das nossa medicoes de tempo.
      
   * Minimizar efeitos de cache nas mediçoes:
     Como faremos a chamada da funcao NTIMES = 1000 vezes para tirar medias,
     e reportar tempo e vazao por chamada a cada rodada do programa
     *conforme feito na versao da reduçao paralela*, 
     TANTO o vetor de entrada Input QUANTO o vetor P, 
     bem como os vetores Output e Pos (ambos de saida) 
     DEVEM SER ser replicados a cada para que se faça um processamento
     em uma copia de cada vetor, a cada nova chamada da funcao de particionamento. 
     Assim evitamos medir errado, quando os efeitos de cache mudam nossos resultados
     de desempenho.  Ver mais explicacoes no final desse arquivo
     
   * Sobre as MEDICOES:
     voce DEVE usar as funcoes de medicao de tempo do prof.
     (chronos)  
   
O que entregar?
---------------

   - um RELATORIO conforme secao (RELATÓRIO) descrita abaixo
   - O seu programa em C (todas as funcoes em apenas 1 arquivo, a menos
     das funcoes de medicao de tempo do prof que DEVE estar em seu arquivo separado)
   . script para compilar
   . scripts para rodar os experimentos 
      (pode se basear nos scripts do prof para rodar com slurm
       MAS voce deve entregar seu(s) scripts adaptados se necessario adaptar algo)     
   . duas planilhas, uma para a parte A, outra para a parte B 
       (DEVE se basear nas planilhas do prof de reducao paralela com pool de threads)
       adaptando a planilha para as nossas 
       experiencias aqui descritas ver item (PLANILHAS) acima.
       
       ATENCAO: A vazao deve ser adaptada: 
             - para calcular numero de Elementos Particionados por Segundo (EPS)
               reportar na escala mais adequada 
               (ESCALA adequada significa: 
               a ESCALA que der menos zeros nos numeros e ESCALAS de graficos)
               exemplo:  GEPS ou MEPS (ou seja giga EPS ou mega EPS)
               OBS: Usaremos GIGA == bilhao == 1000000000 elementos
                    ou MEGA == Milhao == 1000000 elementos 
             
       ATENCAO 2: 
         - voce deve incluir na PRIMEIRA pagina das planilhas a saida do comando 
           lscpu para a maquina usada (nesse caso pode ser da w00)
         - voce deve incluir TAMBEM na ULTIMA pagina das planilhas a saida do comando 
           lstopo para a maquina usada (nesse caso pode ser da w00)
                 (a maquina w00 JÁ TEM o lstopo INSTALADO, 
                 MAS se voce quer esse binario
                 eu coloquei o binario desse programa na ufpr virtual, 
                 basta baixar setar bit de execucao e 
                 rodar lstopo w00-xeon.pdf 
                 ele gera o PDF da topologia do processador)
                 
(RELATÓRIO) 
SOBRE O RELATÓRIO do trabalho
Fazer um relatório (PDF) descrevendo sua implementacao e
com suas conclusões sobre os resultados obtidos. Descreva as características importantes 
da sua CPU usada. Inclua em apendice ao relatório o texto da saida do 
comando lscpu bem como a figura do comando lstopo para sua CPU usada.        
  

A FUNÇÃO a ser paralelizada
---------------------------

  A função multi_partition( ... ) 
  - é uma versão de particionamento para multiplas faixas.
  - Particiona o numeros do vetor Input de tamanho n 
      em faixas determinadas pelo vetor P de tamanho np 
      (que indica as np multiplas particoes, ou seja np faixas)
  - Retorna DOIS vetores de saida:
     - o vetor Output, "particionado" em np faixas
     - o vetor Pos, indicando a posiçao do inicio de cada faixa do vetor Output
     
  - OBS: os elementos do vetor P (particoes) devem estar ordenados
    a sua funcao multi_partition NAO vai perder tempo em verificar se P 
    está ordenado, voce deve passar ele já ordenado para a multi_partition
       
  Exemplo:                .  .  .   .  .        .   .  .  .            .   .          
  long long Input[100] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};
  int n = 14;   // n == numero de elementos em Input e em Output
  long long P[20] = {12, 70, 90, 300};
  int np = 4;   // np == numero de faixas no vetor P
                // nesse caso sao 3 faixas
                // faixa 0: numeros menores que 12
                // faixa 1: numeros no intervalo [12,70)
                // faixa 2: numeros no intervalo [70,90)
                // faixa 3: numeros no intervalo [90,300]
  ao chamar 
     multi_partition( Input, n, P, np, Output, Pos ); 
  
  retorna os vetores de Saida:                                  
    
  long long Output[100] = {8, 4, 7, 11, 3, 7, 7, 13, 44, 46, 44, 100, 100, 110 }; 
  // posicoes de Outut:    0  1  2  3   4  5  6  7   8   9   10  11   12   13    
  //  indiquei acima       |                     |               |                 
  //  as posicoes do       | +-------------------+               |                  
  //  vetor para          ++ |                                   |              
  //  entendimento        |  |  +---+----------------------------+            
                          |  |  |   |  
  unsigned int Pos[20] = {0, 7, 11, 11 };   // indica o inicio de cada faixa de saida           
                
OBS: note que implicitamente temos o tamanho de cada faixa
     // faixa 0: tem tamanho 7-0   (7 numeros)
     // faixa 1: tem tamanho 11-7  (4 numeros)
     // faixa 2: tem tamanho 11-11 (0 numeros)
     // faixa 3: tem tamanho 14-11 (3 numeros)
     
OBS: voce pode assumir que o último valor em P é maior ou igual
     a qualquer dos valores de entrada, ou seja, o vetor de saida
     sempre terá o mesmo tamanho do vetor de entrada  
     
prototipo da funcao:     
    void multi_partition( long long *Input, int n, long long *P, int np, 
                          long long *Output, int *Pos ); 
        
  
SOBRE A funçao de vefificaçao a ser chamada
-------------------------------------------

Incluir ao final do seu programa, ANTES de terminar o main 
(e FORA DE qualquer medicao de tempo),
para rodar APENAS na thread 0 (main), 
a funcao verifica_particoes(…) que DEVE ter o
mesmo prototipo abaixo. Voce deve colocar a funcao 
inicial de verificacao abaixo em um
arquivo chamado verifica_particoes.c com um implementacao SUA
e fazer o include desse arquivo no INICIO do seu programa.
Esta funcao de verificacao a ser incluida e chamada deve verificar as faixa e imprimir 
se o particionamento feito por sua funcao paralela multi_partition
está correto ou nao.

prototipo da funcao de verificacao:     
    void verifica_particoes( long long *Input, int n, long long *P, int np, 
                             long long *Output, int *Pos ); 
                             
// a funcao deve vefificar e imprimir uma dessa mensagens:
      ===> particionamento COM ERROS
ou
      ===> particionamento INCORRETO
      
// Caso queira imprimir mais alguma coisa em caso de erro
//  esteja a vontade      

-----------------------------------
ESCLARECIMENTOS SOBRE MEDICOES CORRETAS SEM EFEITOS DE CACHING (v1.3)
-----------------------------------------------------

Conforme feito na medição da versao da reducao paralela com pool de threads
e explicado em aula

Na primeira vez que usamos o vetor de Input e outros vetores
esses sao lidos da RAM e vão para o cache, teremos o tempo de processamento
 medido T1 para essa vez

Para outras NTIMES-1 vezes que formos chamar a funcao, SE usamos o mesmo 
vetor de entrada Input e vetor de particionamento
e se esses cabem no cache as medidas de T2 a Tn vamos ter tempos 
lendo dados que estão no cache
Entao as medidas de tempo de execução T2 a Tn serão MUITO menores, e nossa média de tempo 
reportada será ERRADA pois nao representa a realidade de 
operação que em geral, nao sao feitas de dados em cache.

Para resolver isso tiramos várias cópias do vetor Input e várias cópias do vetor P
colocando em sequência em DOIS vetorzoes InputG e PG de modo que 
a soma do tamanho de InputG seja MAIOR que tamanho do cache da máquina
TAMBEM o tamanho PG deve ser MAIOR que tamanho do cache da máquina
(ou seja, NAO podem caber nos caches)

devemos fazer essas cópias ANTES de iniciar medições de tempo.

Assim, a cada nova medição de tempo devemos usar a pesquisa em uma cópia diferente
de Input, e uma cópia diferente de P (cada cópia tem o mesmo conteúdo mas estah em 
lugar DIFERENTE da RAM)
com isso garantimos a medida correta e podemos calcular a média correta de tempos
pois a cada execucao os dados de Input e P estarão sendo lidos GARANTIDAMENTE da RAM,
