#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define NUM_STR 100
#define STR_LEN 200

int RegistaMusicas(char Lista[NUM_STR][STR_LEN]) {
    struct dirent *entry;
    DIR *dp = opendir("Playlist");
   

    int num_musicas = 0;


    while (entry = readdir(dp)) {

        if (entry->d_type == DT_REG) { 
            strncpy(Lista[num_musicas], entry->d_name, strlen(entry->d_name) - 4);

            num_musicas++;
        }
    }
    closedir(dp);

    return num_musicas;
}

void ChoiceHandler(char t, int * loop, int * random, int * pausa){
    if(t=='o'){
        (*loop)++;
        (*loop)%=2;
        printf("[%d]Loop\n",*loop);
    }

    if(t=='r'){
        (*random)++;
        (*random)%=2;
        printf("[%d]Aleatório\n",*random);
    }

    if(t=='p'){
        (*pausa)++;
        (*pausa)%=2;
    }

    if(t=='s'||t=='n')
        Mix_HaltMusic();

}

int main() {

    srand(time(NULL));
    SDL_Init(SDL_INIT_AUDIO);
    
    char Lista[NUM_STR][STR_LEN], MusicaRaw[STR_LEN+13], t;
    int num=-1,num_musicas = RegistaMusicas(Lista),loop = 0,pausa=0, random = 0;

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);


    while(num!=0){

        printf("\n\n_____________________________\n%d Músicas Disponíveis\n\n\n",num_musicas);
        if (num_musicas == 0) {return 1;}

        printf("[0: Sair]\n");

        for (int i = 0; i < num_musicas; i++) {
            printf("[%d: %s]\n", i + 1, Lista[i]);
        }

        printf("\n_____________________________\nMúsica escolhida: ");

        if(num<0)
        scanf("%d", &num);

        else
        printf("%d\n",num);

        if(num < 0 || num > num_musicas){return 1;}
        if(num == 0)
            continue;
        

        sprintf(MusicaRaw, "Playlist/%s.wav", Lista[num - 1]);

        Mix_Music *inQueue = Mix_LoadMUS(MusicaRaw);

        if (inQueue != NULL) {


            printf("\n\n[o|loop]\n[r|aleatório]\n\n\n[s|quebra a música atual]\n[p|pausar/continuar]\n[n|próxima]\n\n\n");
            loop?printf("[1]Loop\n"):printf("[0]Loop\n");
            random?printf("[1]Aleatório\n"):printf("[0]Aleatório");
            printf("\n\n\nNow playing: %s\n", Lista[num - 1]);
            printf("_____________________________\n");


            Mix_PlayMusic(inQueue, 0);
    

            while (Mix_PlayingMusic()) {
                t = getchar();
                ChoiceHandler(t,&loop,&random,&pausa);

                if(pausa)
                Mix_PauseMusic();

                if(!pausa)
                Mix_ResumeMusic();

                SDL_Delay(100);
            }



            if(t=='n')
            num==num_musicas?num=1:num++;

            if(random&&!loop)
            num = 1 + rand()%num_musicas;

            if(!(loop||random)||t=='s')
            num=-1;

            t=' ';
            MusicaRaw[0]='\0';



            Mix_FreeMusic(inQueue);     
        }
    }

    Mix_Quit();
    SDL_Quit();

    return 0;
}