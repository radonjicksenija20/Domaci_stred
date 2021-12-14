#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE* fp;
	size_t broj=1;
	char *izbor, *str, *str2;

	
	while(1){
		puts("1: Pročitaj trenutno stanje stringa");
		puts("2: Upiši novi string");
		puts("3: Konkataniraj string na trenutni");
		puts("4: Izbriši čitav string");
		puts("5: Izbriši vodeće i prateće space karaktere");
		puts("6: Izbriši izraz iz stringa");
		puts("7: Izbriši poslednjih n karaktera iz stringa");
		//Citanje vrednosti 
		izbor = (char*) malloc (broj + 1);
		getline(&izbor, &broj, stdin);


		// PAZI TREBA * ISPRED IZBOR
		switch(*izbor)
		{
		case '1':
			fp = fopen ("/dev/stred", "r");
			if(fp==NULL) {
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}	
			str = (char*) malloc(broj+1);
			getdelim(&str, &broj,(int)'\0', fp);
			printf(" %s \n", str);			
			if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
			free(str);
			break;
		case '2':
		        fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			printf("Unesite string: \n");
			str = (char*) malloc(broj+1);
			getline(&str, &broj, stdin);
			printf("String koji unesem je %s \n", str);
			//sscanf(str, "%[^\n]", str);
			fprintf(fp, "string=%s \n", str);
			if(fclose(fp)){
				printf("Problem pri zatvaranju /dev/stred\n");
					return -1;
			}
			free(str);
			break;
		case '3':
			fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			puts("String koji zelite dodati na postojeci: ");
			str = (char*) malloc(broj+1);
			getline(&str,&broj,stdin);
			fprintf(fp, "append=%s\n", str);
			 if(fclose(fp)){
                                printf("Problem pri zatvaranju /dev/stred\n");
                                        return -1;
                        }
                        free(str);
                        break;
		case '4':
			fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			fputs("clear\n",fp);
                        if(fclose(fp)){
                                printf("Problem pri zatvaranju /dev/stred\n");
                                        return -1;
                        }
                        free(str);
                        break;
		case '5':
			fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			fputs("shrink\n",fp);
		       	if(fclose(fp)){
                                printf("Problem pri zatvaranju /dev/stred\n");
                                        return -1;
                        }
                        free(str);
                        break;
		case '6':
			 fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			puts("Koji izraz brisete: ");
			str = (char*) malloc (broj+1);
			getline(&str, &broj, stdin);
                        fprintf(fp, "remove=%s\n", str);
                        if(fclose(fp)){
                                printf("Problem pri zatvaranju /dev/stred\n");
                                        return -1;
                        }
                        free(str);
                        break;
		case '7':
			fp = fopen ("/dev/stred", "w");
                        if(fp==NULL) {
                                puts("Problem pri otvaranju /dev/stred");
                                return -1;
                        }
			puts("Koliko karaktera zelite da obrisete: ");
			str = (char*) malloc (broj+1);
			getline(&str, &broj, stdin);
                        fprintf(fp, "truncate=%s\n", str);
                        if(fclose(fp)){
                                printf("Problem pri zatvaranju /dev/stred\n");
                                        return -1;
                        }
                        free(str);
                        break;
		case 'Q': 
			puts("Izlazimo iz aplikacije");
			return 0;
			break;

		default: 
			break;
			
		}
		free(izbor);
	}
	return 0;
}
		

