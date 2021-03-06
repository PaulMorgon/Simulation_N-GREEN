library(ggplot2)
library(scales)

# On part du principe que les fichiers CSVs sont toujours générés par deux.
nombre_fichiers = length(list.files(path ="../CSV/", pattern="*.csv"))

#Active la création de fichier PDF lorsqu'un plot est executé
pdf("../PDF/distribution_attentes.pdf")

for (numero_fichier in 1:1 )
{
	chemin_fichier = paste("../CSV/attente_anneau", as.character(numero_fichier), sep="")
	chemin_fichier = paste(chemin_fichier, ".csv", sep="")

	#On cmmence avec la lecture du fichiers indiquant les nombres de messages
	donnees = read.csv(chemin_fichier)
	donnees2 = read.csv("../CSV/attente_anneau2.csv")
	donnees3 = read.csv("../CSV/attente_anneau3.csv")

	nombre_tic = comma(donnees$TIC[1])	#Le nombre de tic de la simulation
	nombre_slot = as.character(donnees$nb_slot[1])
	nombre_noeud = as.character(donnees$nb_noeud[1])
	politique_prioritaire = donnees$politique_prioritaire[1]

	############# On écrit un petit message récapitulatif de la simulation #############
	texte_info = ""
	if (politique_prioritaire == 1)
		texte_info = paste(texte_info, "Best effort messages with a C-RAN priority, fill rate : 35%.")
	else
		texte_info = paste(texte_info, "No priority")


	maxi = max(donnees$valeur)
	if (maxi >= 100)
	{
		pas = 10** (ceiling(log10(maxi))-1)
	    intervalle = round(maxi / pas)
	    labels_x = c(0)
	    i = 0
	    for(i in 1:intervalle)
		{
			if ((i+1)*pas < maxi)
			{
				labels_x = c(labels_x, i*pas)
			}
		}
		labels_x = c(labels_x, maxi)
	}
	############# Génération du graphique indiquant le nombre de message ayant attendu #############
	valeur_max = max (c(donnees$valeur, donnees2$valeur, donnees3$valeur) )
	line_colors = c("#009E73", "#D55E00", "#56B4E9")
    p <- ggplot(donnees, aes(x=valeur_max, y=donnees$taux, group = type, colour = type)) +
	geom_line(data=donnees, aes(x=donnees$valeur, y=donnees3$taux, col = "~40% C-RAN=2.5")) +
	geom_line(data=donnees, aes(x=donnees$valeur, y=donnees2$taux, col = "~60% C-RAN=2.5")) +
	geom_line(data=donnees, aes(x=donnees$valeur, y=donnees$taux, col = "~60% C-RAN=12.5")) +
	scale_colour_manual(values=line_colors) +
 	theme(legend.background = element_rect(fill="lightblue", size = 0.5, linetype="solid"), axis.text=element_text(size=7), axis.title=element_text(size=12,face="bold"), plot.caption=element_text(size=8, face="italic")) +
 	theme(axis.text.x = element_text(size=10)) +
 	theme(axis.text.y = element_text(size=10)) +
 	scale_y_continuous(labels = percent, name = "Message distribution")
	if (maxi >= 100)
 	{
		p <- p + scale_x_continuous(name = "Slots", breaks = labels_x)
	}
	else
	{
		p <- p + scale_x_continuous(name = "Slots")
	}
 	p <- p + labs(caption = texte_info)

	print(p)

	###############################################################
	#### On passe au graphique indiquant les moyennes, min, max ###
	###############################################################
	chemin_fichier = paste("../CSV/repartition_attentes", as.character(numero_fichier), sep="")
	chemin_fichier = paste(chemin_fichier, ".csv", sep="")

	#On cmmence avec la lecture du fichiers indiquant les nombres de messages
	donnees = read.csv(chemin_fichier)

	titre = "Temps d'attentes des messages"
	############# Trie des abscisses #############
	donnees$intervalle <- as.character(donnees$intervalle)
	donnees$intervalle <- factor(donnees$intervalle, levels=unique(donnees$intervalle))

	nombre_tic = comma(donnees$TIC[1])	#Le nombre de tic de la simulation

	if (politique_prioritaire == 1)
	{
		p <- ggplot(donnees, aes(x=intervalle, y=valeur, fill=donnees$type_message))
	}
	else
	{
		p <- ggplot(donnees, aes(x=intervalle, y=valeur, fill=donnees$type_valeur))
	}
	p <- p +geom_bar(stat="identity", position = "dodge") +
	scale_size_area("Type de message") +
	ylab("Nombre de TICs d'attentes") +
	xlab("Quantile") +
	geom_text(aes(label=donnees$valeur, y = donnees$valeur), size = 3, position = position_dodge(width =0.9), hjust=0.5, vjust=0.5, color="black") +
	theme(legend.background = element_rect(fill="lightblue", size = 0.5, linetype="solid"), axis.text=element_text(size=7), axis.title=element_text(size=12,face="bold"), plot.caption=element_text(size=8, face="italic"), axis.text.x=element_blank(),) +
	scale_fill_discrete(name = "Type de message") +
	labs(caption = texte_info) +
	ggtitle(titre)

	print(p)
}

dev.off()
q(save="no")
