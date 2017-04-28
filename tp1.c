#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct designant les chiffres
typedef struct cell
{
  char chiffre;
  struct cell *suivant;
} cell;

//struct designant les nombres
typedef struct num
{
  int negatif;
  struct cell *chiffres;
  int compt_ptr;
} num;

//struct designant les operandes
typedef struct operande
{
    struct num *nombre;
    struct operande *suivant;
} operande;

operande* pile_op = NULL; //Pile contenant les operandes lus
num* variables[26]; //Tableau de pointeurs vers des num (0 = a, 26 = z)

// ---- FONCTIONS DE GESTION DE LA MÉMOIRE ----

//Supprime un num et les cell associées
void suppNum(num *ptr){
    
    ptr->compt_ptr--;
    
    if (ptr->compt_ptr == 0) //Si compteur a 0, supprimer le num
     {
        
        while (ptr->chiffres != NULL) //Supprimer toutes les cells associées
        {
            cell* cell_tmp = ptr->chiffres;
            ptr->chiffres = ptr->chiffres->suivant;     
            free(cell_tmp);
            
        }
        
        free(ptr);
        ptr = NULL;
    }


    
}

//Supprime la premiere operande de la pile
void suppOp() {

        suppNum(pile_op->nombre);

        operande *op_tmp = pile_op;    
        pile_op = pile_op->suivant;
        free(op_tmp);
      

}

//Vide toute la pile d'opérandes
void videPileOp()
{
    while (pile_op != NULL)
    {
        suppOp(); //Supprimer l'operande
        
    }
}

//Vide le tableau des variables
void videVariables()
{
    int i;
    for (i = 0; i<26; i++)
    {
        if (variables[i] != NULL)
        {
            suppNum(variables[i]);
            variables[i] = NULL;
        }
    }
}

//Ajoute un nouveau cell au debut de la liste (LIFO)
void pushCellFront(cell **p, char a)
{
    cell *tmp = (cell *)malloc(sizeof(cell));
    
    if (tmp == NULL) 
    {
        printf("ERREUR - Mémoire insuffisante\n");
        exit(EXIT_FAILURE);
    }
    
    tmp->chiffre = a;
    tmp->suivant = *p;
    *p = tmp;
    
    //printf("push %i\n", tmp->chiffre - 48);
}

//Ajouter une nouvelle cell a la fin de liste;
//comme un queue (FIFO)
struct cell *dernier;
void pushCellBack(cell **p, char a)
{
  cell *tmp = (cell *)malloc(sizeof(cell));
  
  if(*p == NULL)
    {
      tmp->chiffre = a;
      tmp->suivant = NULL;
      *p = tmp;
      dernier = tmp;
    }
  else
    {
      tmp->chiffre = a;
      tmp->suivant = NULL;
      dernier->suivant = tmp;
      dernier = tmp;
    }
  //printf("push %i\n", tmp->chiffre - 48);
}

//Ajoute un nouvel operande 
void pushOp(operande **op, num *n) 
{
    operande *tmp = (operande*)malloc(sizeof(operande));
    
    if (tmp == NULL) 
    {
        printf("ERREUR - Mémoire insuffisante\n");
        exit(EXIT_FAILURE);
    }
    
    tmp->nombre = n;
    tmp->nombre->compt_ptr++;
    
    tmp->suivant = *op;
    *op = tmp;
}

// ---- FONCTIONS D'IMPRESSION ----

//Imprime le compteur de référence du dernier operande lu
void printCompteur() 
{
    if (pile_op == NULL)
    {
        printf("ERREUR - opérande manquant (usage: <opérande> ?)\n");
    } else 
    {
        char compteur = pile_op->nombre->compt_ptr-1 + 48;
        suppOp();
        
        //Allouer un nouveau num pour le resultat
        num *n = (num*)malloc(sizeof(num));
        
        if (n == NULL) 
        {
            printf("ERREUR - Mémoire insuffisante\n");
            exit(EXIT_FAILURE);
        }
        
        n->negatif = 0;
        n->chiffres = NULL;
        
        //Ajouter un bloc a la pile d'operandes
        pushOp(&pile_op, n);
        //n->compt_ptr = 1;
        
        pushCellFront(&pile_op->nombre->chiffres, compteur);

    }
}

void printResultat()
{
    if (pile_op->suivant != NULL) 
    {
        printf("ERREUR - le nombre d'operandes et d'operateurs ne concordent pas.\n");
    } else {
        
        cell* print_pile = NULL;
        cell* print_ptr = NULL;

        if (pile_op->nombre->negatif == 1) {
            printf("-");
        }
        
        cell *cell_tmp = pile_op->nombre->chiffres;
        
        if (cell_tmp == NULL) {
            printf("%d",0);
        }
        else
        {
            //Insérer les nombres en ordre d'impression dans une pile temporaire
            while (cell_tmp != NULL) {
            pushCellFront(&print_pile, cell_tmp->chiffre);
            cell_tmp = cell_tmp->suivant;
            }
            
            print_ptr = print_pile;
            
            //Imprimer les nombres et supprimer les cellules au fur et a mesure
            while(print_pile != NULL) {
                printf("%d", print_ptr->chiffre-48);
                print_ptr = print_ptr->suivant;
                free(print_pile);
                print_pile = print_ptr;
            }
        }

        printf("\n");
    }
}



// ----OPÉRATIONS ----

/* Vérifie si le caractère est valide pour une variable
 * Retourne l'index dans le tableau des variables si valide, -1 sinon
*/
int validerVariable(char var) 
{
    int index = (int)var-97;
    if (index < 0 || index > 25) 
    {
        printf("ERREUR - caractère invalide - variable doit être une lettre minuscule de a à z(\n");
        return -1;
    }
    
    return index;
}

//Assigne le dernier opérande lu à une variable
void assignVariable(char var) 
{
    //printf("ASSIGNATION À lA VARIABLE %c\n", var);
    if (pile_op == NULL) 
    {
        printf("ERREUR - aucun opérande à assigner\n");
    } else 
    {
        int index = validerVariable(var);
        if (index != -1) 
        {
            if (variables[index] != NULL) //Variable déjà occupée
            {   
                suppNum(variables[index]);
            }
            
        variables[index] = pile_op->nombre;
        pile_op->nombre->compt_ptr++;
        
        }
    }
}

//Vérifie qu'il y a au moins 2 opérandes dans la pile pour les opérations binaires
int deuxOp() 
{
    return ((pile_op != NULL) && (pile_op->suivant != NULL));
}


int add() 
{
   
   //printf("ADDITION\n");
    // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return 0;
    }
    
    cell *c1 = pile_op->suivant->nombre->chiffres;
    cell *c2 = pile_op->nombre->chiffres;
    
    num* result;

    //Verifie si un des deux operandes est null
    if (c1 == NULL)      //Si c1 = 0, le résultat est c2
      {
        result = pile_op->nombre;
        result->compt_ptr++;
        
      }
    else if (c2 == NULL) // Si c2 = 0 le résultat est c1
      {
        result = pile_op->suivant->nombre;
        result->compt_ptr++;

      }
    else   //Si les deux operandes sont differentes de 0
      {   
        result = (num*)malloc(sizeof(num));
        if (result == NULL)
        {
            printf("ERREUR - Mémoire insuffisante\n");
            exit(EXIT_FAILURE);
        }
        
        result->compt_ptr++;
        
        int op1 = c1->chiffre-48;
        int op2 = c2->chiffre-48;
        
        //mod: modulo, r: retour, s: somme
        int mod = 0, r = 0, s = 0;
        
        do
          {
            s = op1 + op2 + r;
            
            mod = s % 10;
            r = s / 10;
            
            pushCellBack(&result->chiffres, mod+48);
            
            //Verifie si on arrive a la fin de c1
            if (c1 == NULL || c1->suivant == NULL) {
                c1 = NULL;
                op1 = 0;    
            } else {
                c1 = c1->suivant;
                op1 = c1->chiffre-48;
            }

            
            //Verifie si on arrive a la fin de c2
            if (c2 == NULL || c2->suivant == NULL) {
                c2 = NULL;
                op2 = 0;    
            } else {
                c2 = c2->suivant;
                op2 = c2->chiffre-48;
            }
            
          } while (c1 != NULL || c2 != NULL); //boucle arrete a la fin des deux nombres
            
            
          if (r != 0) //S'il y a un reste
        {   
            pushCellBack(&result->chiffres, r+48);
        }
    }
        
    //Supprimer les deux operandes originales
    int i;
    for (i = 0; i < 2; i++) {
        suppOp(pile_op);
    }
    
    pushOp(&pile_op, result);
    result->compt_ptr--; //On retire le pointeur result a la fin de la fonction
    return 1;
 }

int soust()
{
    // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) 
    {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return 0;
    }
    
    cell *c1 = pile_op->suivant->nombre->chiffres;
    cell *c2 = pile_op->nombre->chiffres; 
    
    num* result;
    
    //Verifie si un des deux operandes est null
    if (c1 == NULL)      //Si c1 = 0, le résultat est -c2
      {
        result = pile_op->nombre;
        result->compt_ptr++;
        result->negatif = (result-> negatif == 0) ? 1 : 0;
        
      }
    else if (c2 == NULL) // Si c2 = 0 le résultat est c1
      {
        result = pile_op->suivant->nombre;
        result->compt_ptr++;
      }
    else   //Si les deux operandes sont differentes de 0
      { 
    
        result = (num*)malloc(sizeof(num));
        if (result == NULL)
        {
            printf("ERREUR - Mémoire insuffisante\n");
            exit(EXIT_FAILURE);
        } 
        result->compt_ptr++;
        
        //Trouver plus grand operande
    
        cell* max = NULL;
        cell* min = NULL;
        
        cell *tmp1 = c1;
        cell *tmp2 = c2;
        
        int signe = 0; //0 = positif, 1 = négatif
        
        for (;;) 
        {
            if (tmp1 == NULL && tmp2 != NULL) // c2 plus long => max
            {
                max = c2;
                min = c1;
                signe = 1;
                break;
            }
            else if (tmp2 == NULL && tmp1 != NULL) // c1 plus long => max
            {
                max = c1;
                min = c2;
                signe = 0;
                break;
            }
            else if (tmp1 == NULL && tmp2 == NULL)
            {
                if (max == NULL) //Longueurs et valeurs égales => retourne 0
                {
                    result->chiffres = NULL; 
                    result->compt_ptr++;
                    result->negatif = 0;
                    
                    //Supprimer les deux operandes originales
                    int i;
                    for (i = 0; i < 2; i++) {
                        suppOp();
                    }
        
                    pushOp(&pile_op, result);
                    result->compt_ptr--; //On retire le pointeur result a la fin de la fonction
                    return 1;
                }
                else
                {
                    break; //Prendre max et min existants
                }
            }
            else if (tmp1->chiffre < tmp2->chiffre) // c1 < c2
            {
                max = c2;
                min = c1;
                signe = 1;
            } 
            else if (tmp1-> chiffre > tmp2->chiffre) // c1 > c2
            {
                max = c1;
                min = c2;
                signe = 0;
            }
            
            tmp1 = tmp1->suivant;
            tmp2 = tmp2->suivant;
        }
        
        result->negatif = signe;
        
        int op1 = max->chiffre-48;
        int op2 = min->chiffre-48;
        
        //mod: modulo, r: retour, d: difference
        int r = 0, d = 0;
        
        for (;;)
        {
            op1 -= r;
            
            if (op1 >= op2)
            {
                d = op1-op2;
                r = 0;
            }
            else
            {
                d = (op1+10)-op2;
                r = 1;
            }
           
            
            //Vérifie si on arrive à la fin du chiffre
            
            if (max->suivant != NULL)
            {
                max = max->suivant;
                op1 = max->chiffre-48;
                pushCellBack(&result->chiffres, d+48);
                
            } else
            {
                if (d != 0) //Ne pas imprimer le dernier chiffre si 0
                {
                    pushCellBack(&result->chiffres, d+48);
                }
                break; //Fin de max <=> fin de la soustraction
            }
            
            //Vérifie si on arrive à la fin de min
            
            if (min->suivant != NULL)
            {
                min = min->suivant;
                op2 = min->chiffre-48;
                
            } else
            {
                op2 = 0;
            }
        }
    
    }
    
    //Supprimer les deux operandes originales
    int i;
    for (i = 0; i < 2; i++) 
    {
        suppOp();
    }
    
    
    pushOp(&pile_op, result);
    result->compt_ptr--; //On retire le pointeur result a la fin de la fonction
    return 1;
}

int mult()
{
  
    // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return 0;
    }
    
    operande *tmp_op1 = pile_op->suivant;
    tmp_op1->nombre->compt_ptr++;
    
    operande *tmp_op2 = pile_op;
    tmp_op2->nombre->compt_ptr++;
    
    //Supprimer les deux operandes originales
    int i;
    for (i = 0; i < 2; i++) {
        suppOp();
    }
   
    
    cell *c1 = tmp_op1->nombre->chiffres;
    cell *c2 = tmp_op2->nombre->chiffres;
    cell *tmp_c1 = c1;
    
  num *result;
  int compteur_zero = 0;
  
  if(c1 == NULL || c2 == NULL)
    {        
        result = (num*)malloc(sizeof(num));
        if (result == NULL)
        {
            printf("ERREUR - Mémoire insuffisante\n");
            exit(EXIT_FAILURE);
        }
        
        result->chiffres = NULL;
        result->compt_ptr++;
        result->negatif = 0;
        
        pushOp(&pile_op, result);
        result->compt_ptr--; //On retire le pointeur result a la fin de la fonction
        return 1;
    }
else
    {
        //pour mettre les zeros chq fois qu'on lit un nouveau chiffre
        //de c2 qui est 10 fois plus que celui avant

        
      while(c2 != NULL)  
        {
          //mul:  multiplication, r: retour, mod: modulo
          int mul = 0, r = 0 , mod = 0;
          
          num *n = (num*)malloc(sizeof(num));
          n->negatif = 0; 
          n->chiffres = NULL;
          n->compt_ptr = 1;
          
          pushOp(&pile_op, n);
          
          int i;
          for(i = 0; i < compteur_zero; ++i)
          {
            pushCellBack(&n->chiffres, 48);
          }
          
          while(tmp_c1 != NULL)
            {
              if (c2->chiffre != 0) 
              {
                
                mul = (c2->chiffre - 48) * (tmp_c1->chiffre - 48) + r;
                mod = mul % 10;
                r = mul / 10;
                pushCellBack(&n->chiffres, mod+48);
                tmp_c1 = tmp_c1->suivant;
              }

            }
            
          if (r != 0)
          {
            pushCellBack(&n->chiffres, r+48);
          }

          
          tmp_c1 = c1;
          c2 = c2->suivant;
          ++compteur_zero;
          n->compt_ptr--;
        }
    }
  
  while(compteur_zero-1 > 0)
    {
      add();
      --compteur_zero;
    }
    
    //Nettoyage des allocations temporaires

     suppNum(tmp_op1->nombre);
     if (tmp_op2->nombre != NULL)
    {
        suppNum(tmp_op2->nombre);
    }
       
  return 1;
}

// ---- LECTURE DES ENTRÉES ----

void lireSignesAdd()
{
        // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return;
    }
    
        int signe1 = pile_op->suivant->nombre->negatif;
        int signe2 = pile_op->nombre->negatif;
        
        if (signe1 == signe2)
        {
            add();
            if (pile_op->nombre->chiffres != NULL) // résultat != 0
            {
                pile_op->nombre->negatif = signe1;
            } else
            {
                pile_op->nombre->negatif = 0;
            }
            
        } 
        else
        {
            soust();
            if (pile_op->nombre->chiffres != NULL) // résultat != 0
            {
                pile_op->nombre->negatif = abs(signe1-pile_op->nombre->negatif);
            } else
            {
                pile_op->nombre->negatif = 0;
            }
        }
}

void lireSignesSoust()
{
    // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return;
    }
    
        int signe1 = pile_op->suivant->nombre->negatif;
        int signe2 = pile_op->nombre->negatif;
        
        if (signe1 == signe2)
        {
            soust();
            if (pile_op->nombre->chiffres != NULL) // résultat != 0
            {
                pile_op->nombre->negatif = abs(signe1-pile_op->nombre->negatif);
            } else
            {
                pile_op->nombre->negatif = 0;
            }
            
        } 
        else
        {
            add();
            if (pile_op->nombre->chiffres != NULL) // résultat != 0
            {
                pile_op->nombre->negatif = signe1;
            } else
            {
                pile_op->nombre->negatif = 0;
            }
        }
}

void lireSignesMult()
{
     // Si pas assez d'opérandes, sortir de la fonction
    if (!deuxOp()) {
        printf("ERREUR - Nombre d'opérandes insuffisant. Usage: <operande 1> <operande 2> +");
        return;
    }
        int signe1 = pile_op->suivant->nombre->negatif;
        int signe2 = pile_op->nombre->negatif;
        
        mult();
                    
        if (signe1 == signe2)
        {
            pile_op->nombre->negatif = 0;
        } 
        else
        {
            pile_op->nombre->negatif = 1;
        }
}

//Appelle les fonctions appropriées selon l'opérateur lu
void lireOperateur(char c)
{
    switch (c) {

        
        case '+':
            lireSignesAdd();
            break;
        case '*':
	  lireSignesMult();
            break;
        case '-':
           lireSignesSoust();
            break;
        case '?':
            printCompteur();
            
    }
}

/* Lit le chiffre entré
 * continue_nb détermine si le chiffre est ajouté à la fin d'un nombre en cours (1)
 * ou si un nouvel opérande doit être créé (0)
 */
void lireChiffre(char x, int continue_nb) 
{
   
    if(!continue_nb) //Debut d'un nouvel operande
    {
        //Allouer un nouveau num pour l'operande
        num *n = (num*)malloc(sizeof(num));
        
        if (n == NULL) 
        {
            printf("ERREUR - Mémoire insuffisante\n");
            exit(EXIT_FAILURE);
        }
        
        n->negatif = 0;
        n->chiffres = NULL;
        
        //Ajouter un bloc a la pile d'operandes
        pushOp(&pile_op, n);
        n->compt_ptr = 1;
    }
    
    if (continue_nb || x != 48) //Pas d'ajout de cell si le premier chiffre du nombre est un 0
    {
        //Allouer un nouveau cell pour le chiffre lu
        pushCellFront(&pile_op->nombre->chiffres, x);
    }

}

//Place la variable appelée dans la pile des opérandes
void lireVariable(char var)
{
    int index = validerVariable(var);
    
    if (index != -1) 
    {
        if (variables[index] == NULL) {
            printf("ERREUR - Variable %c non initialisée. Pour assigner une valeur : <expression> =<variable>", var);
        } else {
            //Ajouter un bloc a la pile d'operandes
            pushOp(&pile_op, variables[index]);
            //variables[index]->compt_ptr++;
        }

    }
}

int detectEntier(char c) 
{
    return (c >= 48 && c <=57);
}

int detectAlpha(char c)
{
    return (c >= 97 && c <= 122);
}

int detectOp(char c) 
{
    return (c == 42 || c == 43 || c == 45 || c == 63);
}

int lireEntree() 
{
    char c;
    int continue_nb = 0; //0 = nouveau nb, 1 = continue le nb precedent
    
    while ((c = getchar()) != '\n' && c != EOF) {
        
        if (detectEntier(c)) // entier
        {
            //printf("Trouvé entier : %d\n", c-48);
            lireChiffre(c, continue_nb);
            continue_nb = 1;
            
        } 
        else if (detectOp(c)) // +*-?
        {
            //printf("Trouvé opérateur : %c\n", c);
            lireOperateur(c);
            continue_nb = 0;
        } 
        else if (c == 61)       // =
        {
            //printf("Assignation de variable\n");
            if (detectAlpha(c = getchar())) {
                assignVariable(c);
            } else {
                printf("ERREUR - Variable invalide - Usage: =<variable>\n");
            }
            continue_nb = 0;
        } 
        else if (detectAlpha(c)) //caractère alpha
        {
            //printf("Caractère alpha\n");
            lireVariable(c);
            continue_nb = 0;
        } 
        else if (c == 32) // espace
        {
            //printf("espace\n");
            continue_nb = 0; //prochain nombre commencera un nouvel operande
            
        } else {
            printf("ERREUR - caractère invalide\n");
        }
    }
    
    
    return (!(c == EOF)); // Retourne 1 si terminé normalement, 0 si interrompu par l'usager
}




int main (int argc, char *argv[])
{       
   //Lecture de la ligne de commande - a completer

    
    while (!feof(stdin)) { //Tant qu'on n'obtient pas EOF (ctrl+D)

        
        printf(">");
            
        if (lireEntree()) 
        {
            printResultat();
            
            //Vider le contenu de pile_op à la fin de chaque ligne
            videPileOp();
        }
        

    }
    
    videPileOp();
    videVariables();
    printf("\nProgramme terminé\n");
    return 0;
}
