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
 * @brief Fichier principal du programme, contient la fonction main.
 * @author Hyacinthe Cartiaux
 * @date Novembre / Décembre 2006
 */

/** @mainpage XTM
 *
 * @section intro_sec Introduction
 *
 * Le format de fichier XTM est le format utilisé par le logiciel XTRemSplit. Ce logiciel permet de couper des fichiers en plusieurs fichiers de taille moins importante. Le programme XTM permet de réassembler les fichiers par l'intermédiaire d'une interface en ligne de commande.\n
 * XTM est écrit en C et est distribué sous licence GNU GPL.
 *
 *
 * @section install_sec Installation
 *
 * @subsection method1 Compilation
 *
 * \code $ tar xjvf xtm-0.2.tar.bz2
 * $ cd xtm-0.2
 * $ make
 * # make install \endcode
 *
 *
 * @section usage_sec Utilisation
 *
 * \code xtm -i path/to/file.001.xtm [-o output/directory/] [-f] [-v] \endcode \n
 * Lisez la page de manuel pour plus d'informations
 *
 * @section bug_sec Bug
 *
 * Vous pensez avoir trouvé un bug ? Merci de m'envoyer un mail expliquant comment le reproduire à l'adresse hyacinthe.cartiaux ( a ) free.fr
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xtmfile.h"

/** Affiche des informations sur le programme. */
void show_usage();

/** Affiche les informations d'une structure xtm_info. */
void show_xtminfo(struct xtm_info *xtm_info_struct);

/**
 * Fonction main
 * @return \c EXIT_FAILURE (1) en cas d'échec, \c EXIT_SUCCESS (0) sinon.
 */
int main(int argc, char *argv[])
{
	struct xtm_file *xtm_file_struct;
	struct xtm_info *xtm_info_struct;
	struct xtm_file **xtm_file_struct_tab;
	char **xtm_filenames;
	int number_of_xtm_files;
	int i, nbr_bad_file;

	struct stat stat_buf;
	char *output_file;

	/* Gestion des arguments passés à la commande */

	int c;
	char *filename_001_xtm = NULL;
	char *o_directory;
	extern char *optarg;
	extern int optopt;
	int ask = 1, error = 0, disp_info = 0;

	while ((c = getopt(argc, argv, "i:o:fv")) != -1)
	{
		switch(c)
		{
			case 'f':
				ask = 0;
				break;
			case 'v':
				disp_info = 1;
				break;
			case 'i':
				filename_001_xtm = optarg;
				break;
			case 'o':
				o_directory = optarg;
				break;
			case ':':
				error = 1;
				break;
			case '?':
				error = 1;
				break;
		}
	}


	if (error == 1 || filename_001_xtm == NULL)
	{
		show_usage();

		return EXIT_FAILURE;
	}


	

	/* Récupération de la structure d'info xtm_info du fichier 001.xtm */

	xtm_file_struct = xtm_open_file(filename_001_xtm);

	if (xtm_file_struct != NULL && ( xtm_info_struct = xtm_get_info(xtm_file_struct->fd) ) != NULL)
	{
		/* Le fichier existe et il s'agit bien d'un fichier de type .001.xtm */
		
		printf("%s ... [OK]\n", xtm_file_struct->filename);

		show_xtminfo(xtm_info_struct);


		number_of_xtm_files = xtm_info_struct->number_of_files;

		xtm_free_file_struct(xtm_file_struct);
	}
	else
	{
		/* Le fichier n'existe pas, on quitte */
		
		fprintf(stderr, "%s ... [FAILED]\n", filename_001_xtm);

		xtm_free_file_struct(xtm_file_struct);

		return EXIT_FAILURE;
	}
	
	if (disp_info == 1)
	{
		/* option -v détectée, l'affichage a déjà été effectué, on quitte */
		
		free(xtm_info_struct);
		return EXIT_SUCCESS;
	}



	/* Vérification de o_directory et détermination de output_file */

	if (o_directory != NULL) {

		/* -o a été utilisé */
		
		stat(o_directory, &stat_buf);

		if (S_ISDIR(stat_buf.st_mode))
		{
			/* Le paramètre suivant -o est bien un répertoire, on détermine le chemin du fichier à créer pour l'assemblage */
			
			output_file = (char *) malloc( ( 	strlen(o_directory)
								+ strlen(xtm_info_struct->original_file_name)
								+ 2 )
							  );

			strcpy(output_file, o_directory);
			strcat(output_file, "/");
			strcat(output_file, xtm_info_struct->original_file_name);

			if (stat(output_file, &stat_buf) == 0 && ask == 1)
			{
				/* Si un fichier portant le même nom que le fichier à assembler existe déjà, et que -f n'a pas été passé en paramètre, on demande confirmation */
				
				printf("Erase file %s ? [y/n] ", output_file);
				if (getchar() != 'y')
				{
					printf("Aborting.\n");
					free(output_file);
					free(xtm_info_struct);
					return 1;
				}
				printf("\n");
			}

		}
		else
		{
			/* Le paramètre suivant -o n'est pas un répertoire */

			printf("%s ... [FAILED]", o_directory);
			free(xtm_info_struct);
			return EXIT_FAILURE;
		}

	}
	else
	{
		/* Le nom du fichier à assembler est celui du fichier d'origine, il sera créé dans le répertoire courant */
		
		output_file = (char *) malloc ( ( xtm_info_struct->original_file_name_length + 1) * sizeof(char) );
		strcpy(output_file, xtm_info_struct->original_file_name);
	}



	

	/* Récupération des noms des fichiers xtm */

	xtm_filenames = xtm_get_all_filenames(filename_001_xtm, number_of_xtm_files);




	/* Ouverture de tous les fichiers xtm nécessaires */

	xtm_file_struct_tab = xtm_open_all_files(xtm_filenames);

	nbr_bad_file = 0;

	for (i = 0; i < number_of_xtm_files; i++)
	{
		/* On vérifie chaque fichier */
		
		if (xtm_file_struct_tab[i] != NULL)
		{
			/* Le fichier a bien été ouvert */
			
			printf("%s ... [OK]", xtm_filenames[i]);
		}
		else
		{
			/* Le fichier n'a pas pu être ouvert en lecture, on commence à libérer la mémoire de xtm_file_struct */
			
			fprintf(stderr, "%s ... [FAILED]", xtm_filenames[i]);
			xtm_free_all_file_struct(&xtm_file_struct_tab[nbr_bad_file]);
			nbr_bad_file = i + 1;
		}

		printf("\n");
	}

	xtm_free_all_filenames(xtm_filenames); 	/* On libère la mémoire pour le tableau de noms de fichiers devenu inutile */

	if (nbr_bad_file > 0)
	{
		/* Un ou plusieurs fichiers nécessaires n'ont pas pu être ouvert, on libère la mémoire et on quitte */
		
		free(output_file);
		free(xtm_info_struct);
		xtm_free_all_file_struct(&xtm_file_struct_tab[nbr_bad_file]);
		free(xtm_file_struct_tab);
		return EXIT_FAILURE;
	}





	/* Phase d'assemblage des fichiers */

	printf("Assembling xtm files ...\n");

	if (xtm_assemble(xtm_info_struct, xtm_file_struct_tab, output_file) == 0)
	{
		/* L'assemblage s'est bien déroulé. */
		
		printf("%s ... [OK]\n", output_file);
	}
	else
	{
		/* L'assemblage a échoué, on quitte et on libère la mémoire */
		
		fprintf(stderr, "%s ... [FAILED]\n", output_file);

		free(output_file);
		free(xtm_info_struct);
		xtm_free_all_file_struct(xtm_file_struct_tab);
		free(xtm_file_struct_tab);

		return EXIT_FAILURE;
	}

	/* Libération de la mémoire */
	
	free(output_file);
	free(xtm_info_struct);
	xtm_free_all_file_struct(xtm_file_struct_tab);
	free(xtm_file_struct_tab);

	/* Sortie du programme */

	return EXIT_SUCCESS;

}

void show_usage()
{
	printf("XTM v0.2\n");
	printf("Copyright 2006 Hyacinthe Cartiaux\n");
	printf("This program is free software; you may redistribute it under the terms of\nthe GNU General Public License.  This program has absolutely no warranty.\n<http://www.gnu.org/licenses/gpl.html>\n\n");
	printf("Usage : xtm -i path/to/file.001.xtm [-o output/directory/] [-f] [-v]\n");
	printf("-f : Do not prompt for confirmation if the new file exists.\n");
	printf("-v : Display headers informations of the 001.xtm file and quit.\n");
}

void show_xtminfo(struct xtm_info *xtm_info_struct)
{
	printf("Number of files : %d\n", xtm_info_struct->number_of_files);
	printf("Original file name : %s\n", xtm_info_struct->original_file_name);
	printf("Original file size : %ld\n", xtm_info_struct->original_file_size);
	printf("Md5 sum enabled : %d\n\n", xtm_info_struct->md5_hash_enabled);
}
