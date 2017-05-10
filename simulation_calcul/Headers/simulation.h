#include "./ListeChainee.h"
#include "./hyper_expo.h"

/*!
 * \file simulation.h
 * \brief Header du fichier principal contenant les fonctions et structures necessaires au main.
 */


/*! \def NOMBRE_TIC
 * \brief Représente le nombre de TIC sur lequel portera la simulation.
 */
#define NOMBRE_TIC 100000	/* NE PAS ALLER AU DELA DE 500 000 TIC !!! */

/*! \def NOMBRE_SLOT
 * \brief Indique le nombre de slot de l'anneau.
 */
#define NOMBRE_SLOT 25

/*! \def NOMBRE_NOEUDS
 * \brief Indique le nombre de noeud de l'anneau.
 */
#define NOMBRE_NOEUD 10

/*! \def PERIODE_MESSAGE_ANTENNE
  * \brief Indique la période selon laquelle les antennes enverront des messages aux noeuds.
  */
#define PERIODE_MESSAGE_ANTENNE 100

/*! \def LIMITE_NOMBRE_MESSAGE_MIN
 * \brief La limite minimale avant un envoi de message depuis un noeud.
 */
#define LIMITE_NOMBRE_MESSAGE_MIN 60

/*! \def LIMITE_NOMBRE_MESSAGE_MAX
 * \brief La limite maximale avant un envoi de message depuis un noeud.
 */
#define LIMITE_NOMBRE_MESSAGE_MAX 80

//////////////////////////////////////////////////
///////////////// Les structures /////////////////
//////////////////////////////////////////////////

/*! \struct PaquetMessage
 * \brief Structure représentant un paquet de message curculant dans les slots.
 * Cette structure contient l'indice du noeud emetteur du paquet, le nombre de message, et un tableau des différents messages.
 */
struct PaquetMessage
{
	int indice_noeud_emetteur;
	int nombre_messages;
	int messages[80];		//Un slot ne peut contenir plus de 80 messages (Un message contient uniquement son tic d'arrivé).
};
typedef struct PaquetMessage PaquetMessage;

/*! \struct Noeud
 * \brief Structure représentant un Noeud de l'anneau.
 * Elle contient un nombre de message, les indices des slots avec lesquels le noeud peut intéragir, ainsi que le nombre d'antenne liée au noeud et le décalage avec laquelle le noeud doit recevoir un message de ses antennes.
 */

struct Noeud
{
	int id;
	int nb_message;
	int indice_slot_lecture;
	int indice_slot_ecriture;
	int nb_antenne;		//Indique le nombre d'antenne auquel est lié le noeuds
	int debut_periode;		//Le décalage selon lequel le noeud recoit des messages des antennes
	ListeChainee *messages;		//File FIFO contenant les TIC d'arrivé des messages
	int attente_max;		//Le temps d'attente maximal dans le noeud
	double nb_message_total;	//Le nombre de message ayant transité dans le noeud
	double attente_totale;		//Le temps d'attente total des messages
};

typedef struct Noeud Noeud;

/*! \struct Slot
 * \brief Structure représentant un slot dans l'anneau.
 * Un Slot est caractérisé par un id (unique), un booléen insiquant si le slot contient un message ou non, ainsi que les indices du noeud lié au slot s'il y en a un.
 */

struct Slot
{
	int id;
	int contient_message;		//Indique si le slot contient un message.
	PaquetMessage *paquet_message;		//Le pquet de message du slot.
	int indice_noeud_lecture;		//Si le slot ne peut accèder a aucun noeud, ce champs vaut -1.
	int indice_noeud_ecriture;		//Si le slot ne peut accèder a aucun noeud, ce champs vaut -1.
};
typedef struct Slot Slot;


//////////////////////////////////////////////////
///////////////// Les fonctions /////////////////
//////////////////////////////////////////////////

/*! \fn void afficher_slots( Slot *slots[] )
 * \brief Initialise les noeuds de l'anneau et modifie en conséquence les indices en écriture/lecture des slots.
 * \param *slots[] Un tableau de pointeur qui contiendra les adresses des slots initialisés.
 */
void afficher_slots( Slot *slots[] );

/*! \fn void initialiser_slots( Slot *slots[] )
 * \brief Initialise les slots de l'anneau.
 * \param *slots[] Un tableau de pointeur sur les slots à initialiser.
 */
void initialiser_slots( Slot *slots[] );

/*! \fn void afficher_noeuds( Noeud *noeuds[] )
 * \brief Initialise les noeuds de l'anneau et modifie en conséquence les indices en écriture/lecture des slots.
 * \param *noeuds[] Un tableau de pointeur qui contiendra les adresses des noeuds initialisés.
 */
void afficher_noeuds( Noeud *noeuds[] );

/*! \fn void initialiser_noeuds( Noeud noeuds[], Slot slots[] )
 * \brief Initialise les noeuds de l'anneau.
 * \param *noeuds[] Un tableau de pointeur sur les noeuds à initialiser.
 * \param *slots[] Un tableau de pointeur vers les slots à modifier.
 */
void initialiser_noeuds( Noeud *noeuds[], Slot *slots[] );

/*! \fn void entrer_messages( Slot slots[], Noeud noeuds[], int tic );
 * \brief Fait entrer des messages dans les noeud selon l'hyper exponentielle et les place dans les slots si cela doit se faire.
 * \param *noeuds[] Un tableau de pointeur sur les noeuds qui recevront les messages.
 * \param *slots[] Un tableau de pointeur sur les slots qui recevront les messages des noeuds.
 * \param tic le tic actuel de l'anneau.
 */
void entrer_messages( Slot *slots[], Noeud *noeuds[], int tic );

/*! \fn void placer_message( Noeud noeud, int indice_noeud_emetteur, Slot *slot, int nombre_message, int messages[], int tic )
 * \brief Transmet un paquet de message d'un noeud vers son slot d'écriture.
 * \param *noeud Un pointeur vers le noeud qui transmet le paquet.
 * \param indice_noeud_emetteur L'indice du noeud qui envoie le message.
 * \param *slot Un pointeur sur le slot qui recevra le paquet de message.
 * \param nombre_message Le nombre de message qui doit etre transmis.
 * \param messages Les messages du paquet. (contiennent le tic d'arrivé dans le noeud).
 * \param tic le tic actuel de l'anneau.
 */
void placer_message( Noeud *noeud, int indice_noeud_emetteur, Slot *slot, int nombre_message, int messages[], int tic );

/*! \fn void decaler_messages( Slot *slots[] )
 * \brief Décale les paquets de mesage des slots dans l'anneau (décalage vers la gauche).
 * \param *slots[] Le tableau de pointeur sur les slots pour lequelles on décale les messages.
 */
void decaler_messages( Slot *slots[] );

/*! \fn void sortir_messages( Slot slots[] )
* \brief Fait sortir des slos les paquets de message lorsqu'ils sont en face de leur noeud emetteur.
* \param *slots[] Le tableau de pinteur sur les slots.
*/
void sortir_messages( Slot *slots[] );

/*! \fn void liberer_memoire( Slot *slots[], Noeud *noeuds[] )
* \brief Libère la mémoire prise durant l'exécution du programme.
* \param *slots[] L'espace mémoire des slots à libérer.
* \param *noeuds[] L'espace mémoire des noeuds à libérer.
*/
void liberer_memoire( Slot *slots[], Noeud *noeuds[] );

/*! \fn void get_temps_attente_max( Noeud *noeuds[], int resultats[] )
* \brief Place dans le tableau resultats les temps d'attentes maximums des messages des noeuds.
* \param *noeuds[] Les noeuds de l'anneau.
* \param resultats[] Le tableau qui contiendra les temps d'attentes maximums.
*/
void get_temps_attente_max( Noeud *noeuds[], double resultats[] );

/*! \fn void get_temps_attente_moyen( Noeud *noeuds[], double resultats[] )
* \brief Place dans le tableau resultats les temps d'attentes moyens des messages des noeuds.
* \param *noeuds[] Les noeuds de l'anneau.
* \param resultats[] Le tableau qui contiendra les temps d'attentes moyens.
*/
void get_temps_attente_moyen( Noeud *noeuds[], double resultats[] );

/*! \fn void ecrire_etat_noeud( Noeud *noeuds[] )
* \brief Ecrit un fichier .dat qui contiendra les données des temps d'attentes des noeuds.
* \param *noeuds[] Les noeuds de l'anneau.
*/
void ecrire_etat_noeud( Noeud *noeuds[] );

/*! \fn void afficher_graphique_attente()
* \brief Affiche via Gnuplot un graphique avec les données du fichier 'attente.dat'.
*/
void afficher_graphique_attente();