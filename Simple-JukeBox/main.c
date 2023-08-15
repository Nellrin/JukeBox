#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define NUM_STR 100
#define STR_LEN 200

int RegistaMusicas(char Lista[NUM_STR][STR_LEN]) {
    struct dirent *entry;
//      aqui vamos assumir que entry vai ser 1 ficheiro de 1 certa diretoria, ambos a serem definidos
    DIR *dp = opendir("Playlist");
/*
        aqui defino que d+ vai assumir o pointer para a diretoria "Playlist" com a função opendir
        para mais tarde, registar o nome de todos os ficheiros desta iterativamente com o entry 
*/      

    int num_musicas = 0;
//      define quantas músicas foram registadas :!

    while (entry = readdir(dp)) {
/*
        aqui definimos entry como o primeiro ficheiro da diretoria a que o pointer dp está associado.
        Como estamos a definir isto dentro da condição do while, o entry será sempre o"próximo" ficheiro
        da diretoria.
        Por cada iteração do loop, o entry vai tomar um ficheiro diferente.
        Ex:

        1ª Iteração
        |1.wav| <- Entry
        |2.wav|
        |3.wav|

        2ª Iteração
        |1.wav|
        |2.wav| <- Entry
        |3.wav|

        3ª Iteração
        |1.wav|
        |2.wav|
        |3.wav| <- Entry

        3ª Iteração
        |1.wav|
        |2.wav|
        |3.wav|
                <- Entry

        Antes da 4ª iteração, readdir(dp) retornará NULL, assim o entry assumirá este valor e o loop acabará.
        A condição "entry = readdir(dp)" deixará de ser verdadeira pois o entry não assumiu nenhum pointer
        diferente de NULL (não há mais ficheiros a ler...).

*/
        if (entry->d_type == DT_REG) { 
/*
        esta condição serve para o array só aceitar ficheiros que contenham informação
        (Ex: imagens, áudios, textos, etc.)
        Ficheiros que não sejam deste tipo (Ex: pastas,etc.) não serão aceites no array
    
*/
            strncpy(Lista[num_musicas], entry->d_name, strlen(entry->d_name) - 4);
/*
        strmcpy(Destino,Fonte,length)
        esta função irá passar o nome de cada ficheiro para o array
        (tendo em atenção que decidi não passar os últimos 4 chars de cada string
        para não mostrar as músicas como *.wav)

        O ficheiro "Good Morning.wav" estará no array como "Good Morning";
*/
            num_musicas++;
//      +1 música registada por iteração 
        }
    }
    closedir(dp);
/*
        não é necessário usar closedir, mas como esta função existe, vou usá-la
        (como quem usa free() ao trabalhar com mallocs, vou usar closedir() depois de usar opendir())
        (e é uma boa prática)
        
*/
    return num_musicas;
//      depois de registar x músicas, retorna o x para ser usado mais tarde
}
int main() {
    
    SDL_Init(SDL_INIT_AUDIO);
/*
        a biblioteca usada neste ficheiro serve para áuudio e muitos outros usos.
        A função SDL_Init() deve ser utilizada antes de qualquer outra das bibliotecas SDL2 e SDL_mixer.
        ao usar SDL_INIT_AUDIO como argumento, estou a dizer que quero iniciar a parte relacionada ao
        áudio destas bibliotecas
*/

    char Lista[NUM_STR][STR_LEN];
/*
        começo um array, capaz de receber os nomes das primeiras 100 músicas da diretoria "Playlist/" 
        estas têm de ter menos de 200 chars. 
*/
    
    int num_musicas = RegistaMusicas(Lista);
/*
        inicio a função que registará os nomes das músicas na diretoria "Playlist/"
        e determino o número de músicas lá presentes
*/

    printf("%d Músicas Disponíveis\n\n",num_musicas);
//      Indicar quantas músicas estão presentes
    if (num_musicas == 0) {return 1;}
// se há um total de 0 músicas presentes, trava o programa


    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
/*
        esta função serve para configurar como é que o output do áudio deve ser
        a taxa de frequência do áudio (44100Hz)
        o formato de áudio (MIX_DEFAULT_FORMAT)
        o número de canais de áudio (2 = stereo / 1 = mono/ 8 para áudios mais especiais)
        o espaço na memória usado para armazenar temporariamente os dados da música antes de serem reproduzidos (2048)

        para uma melhor apreciação de qualquer áudio utilizado, recomendo alterar as configurações definidas para
        Mix_OpenAudio(4000, AUDIO_U8, 2, 2048);
        não te vais arrepender
*/

    for (int i = 0; i < num_musicas; i++) {
        printf("%d: %s\n", i + 1, Lista[i]);
    }
//      vai imprimir iterativamente cada string do array

    int num;
    printf("\nMúsica escolhida: ");
    scanf("%d", &num);
//      deixa o utilizador escolher um número que deve estar associado a uma string 

    if(num < 1 || num > num_musicas){
        return 1;
    }
/*
        devido a um erro inesperado, em vez de ter um loop que pergunta constantemente
        ao utilizador que música deseja ouvir até este escolher um número associado a
        uma música, tenho uma condição que, caso o número escolhido não esteja associado
        a uma música, pára o programa.
*/

    char MusicaRaw[STR_LEN+13];
    sprintf(MusicaRaw, "Playlist/%s.wav", Lista[num - 1]);
/*
        defino uma nova string que vai conter o nome do ficheiro escolhido
        com a diretoria ("Playlist/) e o seu tipo adicionado (.wav)
        sprintf(Destino,Fonte)
*/

    Mix_Music *inQueue = Mix_LoadMUS(MusicaRaw);
/*
        o tipo Mix_Music pertence à biblioteca SDL_mixer e (representa/contém informações sobre) o ficheiro escolhido. 
        É assim um pointer para o ficheiro apresentado pela função Mix_LoadMUS().
        Esta recebe como argumento a diretoria do ficheiro e o seu nome (daí a "MusicaRaw" ter o formato "Playlist/%s.wav")


        char MusicaEscolhida = On Sight
        char MusicaRaw[213]
        MusicaRaw=("Playlist/%s.wav",MusicaEscolhida)
        MusicaRaw="Playlist/On Sight.wav"

        Mix_LoadMUS(MusicaRaw) = Mix_LoadMUS("Playlist/On Sight.wav") = ?

        Diretoria "Playlist/"
            Playlist|Big Iron.wav
                    |On Sight.wav <---
                    |etc.

        Mix_LoadMUS("Playlist/On Sight.wav") = On Sight.wav
        *inQueue = On Sight.wav


        caso a música escolhida tenha sido removida enquanto o programa foi executado,
        a variável inQueue terá um pointer "NULL" (não assumiu nenhum valor/música)
*/
    if (inQueue != NULL) {
        printf("Now playing: %s\n", Lista[num - 1]);
        Mix_PlayMusic(inQueue, 0);

/*
        caso o inQueue contenha informação de uma música, vai mostrar a música a ser tocada
        e o Mix_PlayMusic() vai tocá-la.
        Caso o int seguido do inQueue seja -1, a música será tocada num loop infinito,
        caso fosse 0, a música será tocada 1 vez, 
        caso fosse um número positivo x qualquer, a música será tocada e terá x loops

        Mix_PlayMusic(___,-1)
        Play + Play +Play + Play + Play + Play + Play + Play + Play + Play + Play + etc.

        Mix_PlayMusic(___,0)
        Play

        Mix_PlayMusic(___,2)
        Play + Play + Play

        + = loops
*/

        int count = 0; 
        while (Mix_PlayingMusic()) {
            printf("\n%d:%.2d",count/60,count%60);
            count ++;
            SDL_Delay(1000);

        }
/*
        Mix_PlayingMusic() diz se a música já acabou ou não.
        Enquanto a música não acaba, cada iteração do loop será feita de 1000 em 1000 milissegundos.
        o SDL_Delay deixa o programa "Descansar" durante 1000 milissegundos para não exigir muito do CPU
        (TL:DR, pelo loop, o CPU tem de estar sempre a verificar se continua o loop ou se deixa passar para
        a próxima etapa, e para não verificar o mais rapidamente possível, dei um intervalo de 1000 milissegundos)

        (para ajudar quem não entende tanto disto, 1000 milissegundos = 1 segundo)

        o count vai adicionar 1 por cada iteração e a seguir o programa vai dar printf da divisão do count por 60
        seguida pelo resto do count por 60
*/

        Mix_FreeMusic(inQueue);
/*
        quando a música acabar vamos liberar a memória da música alocada
        Mix_FreeMusic() = free()
*/      
    }

    Mix_Quit();
    SDL_Quit();

/*
        antes de fechar o programa, chamamos as funções Mix_Quit() e SDL_Quit() para liberar memória alocada
        (este é um programa mais simples e direto, mas em programas maiores ;) poderá trazer problemas não usar
        estas funções
*/

    return 0;
}