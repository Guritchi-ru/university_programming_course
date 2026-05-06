#include "compgame.h"

const char* game_titles[] = {"The Witcher 3","Cyberpunk 2077", "Minecraft",
    "Grand Theft Auto V","Red Dead Redemption 2","Hollow knight","God of War","Elden Ring","Tekken8","Call of Duty","Deep Rock Galactic","Portal","World of Warcraft","Dark Souls","Half-Life 2"
};

const char* game_genres[] = {"RPG","Экшн", "Выживач","Шутер","Стратегия","Симулятор","Метроидвания","Рогалик","Хоррор","Кооператив","Платформер","Головоломка"
};

void info(struct description pcgames[], int n){
    int num_titles = sizeof(game_titles) / sizeof(game_titles[0]);
    int num_genres = sizeof(game_genres) / sizeof(game_genres[0]);
    
    for (int i = 0; i < n; i++) {
        strcpy(pcgames[i].title, game_titles[rand() % num_titles]);
        strcpy(pcgames[i].genre, game_genres[rand() % num_genres]);
        
        pcgames[i].release = 2000 + rand()%27;
        pcgames[i].rating = 1.0f + (rand() % 90) / 10.0f;
    }
}

void print_games(struct description pcgames[], int n){
    for (int i=0; i<n; i++){
        printf("Games[%d/%d]: Title: %-25s| genre: %-15s| release: %-10d| rating: %.1f", i+1, n,
        pcgames[i].title, pcgames[i].genre, pcgames[i].release, pcgames[i].rating);
        printf("\n");
    }
}

void SelectSort(struct description pcgames[], int n){
    for (int i=0; i<(n-1); i++){
        int k=i;
        for (int j=i+1; j<n; j++){
            if (pcgames[j].release < pcgames[k].release){
                k=j;
            }
        }
        if (k != i){
            struct description temp = pcgames[i];
            pcgames[i]=pcgames[k];
            pcgames[k]=temp;
        }
    }
}
