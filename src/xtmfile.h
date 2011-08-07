/***************************************************************************
 *   Copyright (C) 2006 by Hyacinthe Cartiaux   *
 *   hyacinthe.cartiaux ( a ) free.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief Fichier de header, fournit les déclarations de fonctions de xtmfile.c.
 * @author Hyacinthe Cartiaux
 * @date Novembre / Décembre 2006
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/** Taille de bloc utilisée pour la copie des fichiers pendant l'assemblage. */
#define BLK_SIZE 4096

/**
 * @brief Structure contenant les informations de l'en-tête d'un fichier .001.xtm.
 * @see xtm_get_info
 */
struct xtm_info {
	/** Nombre de caractères de \c program_name. */
	uint8_t program_name_length;
	/** Nom du programme utilisé pour créer le fichier. */
	char program_name[20];
	/** Nombre de caractères de la chaîne de caractère \c version. */
	uint8_t version_length;
	/** Version du programme utilisé pour créer le fichier. */
	char version[4];
/*	double date; */
	/** Nombre de caractères de \c original_file_name. */
	uint8_t original_file_name_length;
	/** Nom du fichier original. */
	char original_file_name[50];
	/** Indique si les sommes md5 des fichiers se trouvent à la fin du dernier fichier .xtm. Vaut 1 si elles sont présentes, 0 sinon. La somme md5 du dernier fichier est faite sans les bits utilisés pour stocker les sommes md5. */
	uint8_t md5_hash_enabled;
	/** Nombre de fichier xtm. */
	uint32_t number_of_files;
	/** Taille du fichier original. */
	uint64_t original_file_size;
};

/**
 * @brief Structure permettant de stocker les informations d'un fichier .xtm.
 * @see xtm_open_file
 */
struct xtm_file {
	/** Nom du fichier. */
	char *filename;
	/** Taille du fichier. */
	int filesize;
	/** Pointeur de fichier. */
	FILE *fd;
};

/**
 * Permet de récupérer les informations des en-têtes d'un fichier 001.xtm.
 * @param fd Pointeur sur un fichier 001.xtm.
 * @return Un pointeur sur une structure xtm_info, \c NULL si la lecture des informations est impossible.
 * @see xtm_open_file
 */
struct xtm_info * xtm_get_info(FILE *fd);


/**
 * Ouvre un fichier en mode "rb".
 * @param filename Nom du fichier à ouvrir.
 * @return Retourne une structure xtm_file contenant les informations sur le fichier ouvert ou \c NULL si le fichier n'a pas pu être ouvert.
 * @see xtm_free_file_struct
 * @see xtm_open_all_files
 */
struct xtm_file * xtm_open_file(char *filename);

/**
 * Ferme le fichier et libère la mémoire allouée pour une structure xtm_file.
 * @param xtm_file_struct Pointeur vers une structure xtm_file.
 * @see xtm_open_file
 */
void xtm_free_file_struct(struct xtm_file *xtm_file_struct);


/**
 * Ouvre tous les fichiers donnés en paramètre.
 * @param filenames Tableau de chaînes de caractères contenant les noms des fichiers xtm. Le tableau doit se terminer par \c NULL.
 * @return Un tableau de pointeur sur des structures xtm_file correspondant aux noms de fichiers passés en paramètre, dans le même ordre.\n Le tableau se termine par \c NULL. \n Si un fichier n'a pas pu être ouvert, le pointeur correspondant à sa position dans le tableau est \c NULL.
 * @see xtm_free_all_file_struct
 * @see xtm_open_file
 */
struct xtm_file ** xtm_open_all_files(char **filenames);

/**
 * Ferme les fichiers et libère la mémoire allouée pour les structures xtm_file pointées par les éléments du tableau.\n La fonction s'arrête lorsqu'elle rencontre un pointeur \c NULL dans le tableau \c xtm_file_struct_tab.
 * @param xtm_file_struct_tab Tableau de pointeurs sur des structures xtm_file. Le tableau doit se terminer par \c NULL.
 * @see xtm_open_all_files.
 * @see xtm_free_file_struct
 */
void xtm_free_all_file_struct(struct xtm_file **xtm_file_struct_tab);

/**
 * Permet de convertir un nombre en chaîne de caractères.
 * @param nbr Nombre entre 1 et 999.
 * @return Chaîne de 3 caractères dans le format xxx où xxx est un nombre de trois chiffres comprenant les zéros inutiles. Retourne \c NULL si le nombre passé en paramètre n'appartient pas à l'intervale [1;999].
 */
static inline char * xtm_nbr_to_string(int nbr);

/**
 * Génère les noms des fichiers xtm.
 * @param filename_001_xtm Nom du premier fichier .001.xtm.
 * @param nbr_of_files Nombre de fichiers xtm.
 * @return Tableau de chaîne de caractères, chaque chaîne est le nom d'un fichier xtm. Le tableau se termine par \c NULL.
 * @see xtm_free_all_filenames.
 */
char ** xtm_get_all_filenames(char *filename_001_xtm, int nbr_of_files);

/**
 * Libère la mémoire allouées pour un tableau de chaîne de caractères et les chaînes de caractères qu'il pointe.
 * @param xtm_filenames Tableau de chaînes de caractères, terminé par la valeur \c NULL.
 * @see xtm_get_all_filenames.
 */
void xtm_free_all_filenames(char **xtm_filenames);


/**
 * Assemble les fichiers en .xxx.xtm.
 * @param xtm_info_struct Pointeur sur une structure xtm_info extraite du premier fichier xtm avec la fonction \c xtm_get_info.
 * @param xtm_files_tab Tableau de pointeurs sur des structures xtm_file, obtenu avec la fonction \c xtm_open_all_files et terminé par \c NULL.
 * @param new_file Nom du fichier d'origine (xtm_info.original_file_name). Si ce fichier existe, il sera écrasé.
 * @return 0 si l'assemblage a bien été effectué, 1 en cas d'erreur.
 * @see xtm_open_all_files
 * @see xtm_get_info
 */
int xtm_assemble(struct xtm_info *xtm_info_struct, struct xtm_file **xtm_files_tab, char *new_file);
