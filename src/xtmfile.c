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
 * @brief Fournit des fonctions pour assembler des fichiers XTremSplit.
 * @author Hyacinthe Cartiaux
 * @date Novembre / Décembre 2006
 */

#include "xtmfile.h"

struct xtm_info * xtm_get_info(FILE *fd)
{
	struct xtm_info *info;
	
	if (fd == NULL)
		return NULL;
	
	info = (struct xtm_info *) malloc(sizeof(struct xtm_info));
	
	fread(&(info->program_name_length), 1, 1, fd);

	memset(info->program_name, 0, 20);
	fread(info->program_name, info->program_name_length, 1, fd);

	
	fseek(fd, 21, SEEK_SET);

	fread(&(info->version_length), 1, 1, fd);

	memset(info->version, 0, 4);
	fread(info->version, info->version_length, 1, fd);
	

	/* /!\ Buggé, la date est stockée au format TDateTime de Delphi, qui est un double (8 octets)
	 * La partie entière représente le nombre de jour depuis le 30/12/1899.
	fseek(fd, 37, SEEK_SET);
	fread(&(info->date), 3, 1, fd);
	 */

	fseek(fd, 40, SEEK_SET);
	
	fread(&(info->original_file_name_length), 1, 1, fd);

	memset(info->original_file_name, 0, 50);
	fread(info->original_file_name, info->original_file_name_length, 1, fd);


	fseek(fd, 91, SEEK_SET);
	
	fread(&(info->md5_hash_enabled), 1, 1, fd);

	fread(&(info->number_of_files), 4, 1, fd);

	fread(&(info->original_file_size), 8, 1, fd);


	/* vérification des en-têtes */

	if (strcmp("Xtremsplit", info->program_name) != 0)
		return NULL;
	else if (info->original_file_name_length > 49)
		return NULL;
	else if (info->md5_hash_enabled != 0 && info->md5_hash_enabled != 1)
		return NULL;
	else if (info->number_of_files < 2 && info->number_of_files > 999)
		return NULL;
	else 
		return info;

}

struct xtm_file * xtm_open_file(char *filename)
{
	FILE *fd;
	struct xtm_file *file = NULL;
	struct stat buf;

	stat(filename, &buf);
	
	if ((fd = fopen(filename, "rb")) != NULL && buf.st_size > 104 && S_ISREG(buf.st_mode))
	{

		file = (struct xtm_file *) malloc(sizeof(struct xtm_file));
		file->fd = fd;
		file->filename = filename;
		file->filesize = buf.st_size;
		
	}

	return file;

}

void xtm_free_file_struct(struct xtm_file *xtm_file)
{
	if (xtm_file != NULL)
	{
		fclose(xtm_file->fd);
		free(xtm_file);
	}

}

struct xtm_file ** xtm_open_all_files(char **filenames)
{
	struct xtm_file **xtm_file_struct_tab;
	char **current;
	int i = 0;

	for (current = filenames; *current != NULL; current++)
		i++;

	xtm_file_struct_tab = (struct xtm_file **) malloc((i + 1) * sizeof(struct xtm_file *));
	
	xtm_file_struct_tab[i] = NULL;
	
	for (i = 0; filenames[i] != NULL; i++)
	{
		xtm_file_struct_tab[i] = xtm_open_file(filenames[i]);
	}

	return xtm_file_struct_tab;
}

void xtm_free_all_file_struct(struct xtm_file **xtm_file_struct_tab)
{
	struct xtm_file **current;

	for (current = xtm_file_struct_tab; *current != NULL; current++)
		xtm_free_file_struct(*current);
	/* free(xtm_file_struct_tab); */
}

static inline char * xtm_nbr_to_string(int nbr)
{
	char *str;
	int centaines = 0, dixaines = 0, unites = 0;

	if (nbr < 1 || nbr > 999)
		return NULL;
				
	if (nbr >= 100)
	{
		centaines = (nbr - (nbr % 100)) / 100;
		nbr -= centaines * 100;
	}
	
	if (nbr >= 10)
	{
		dixaines = (nbr - (nbr % 10)) / 10;
		nbr -= dixaines * 10;
	}

	unites = nbr;
	
	str = (char *) malloc(4 * sizeof(char));

	str[0] = 48 + centaines;
	str[1] = 48 + dixaines;
	str[2] = 48 + unites;
	str[3] = '\0';
	
	return str;
}

char ** xtm_get_all_filenames(char *filename_001_xtm, int nbr_of_files)
{
	char **xtm_files;
	char *filename_xtm;
	char *nbrstr;
	int filename_len;
	int i;

	
	filename_len = strlen(filename_001_xtm);

	if (filename_len < 9)
		return NULL;
	else if (strcmp("001.xtm", &(filename_001_xtm[filename_len - 7])) != 0)
		return NULL;

	
	xtm_files = (char **) malloc((nbr_of_files++) * sizeof(char *));
	
	for (i = 1; i < nbr_of_files; i++)
	{
		filename_xtm = (char *) malloc(filename_len * sizeof(char));
		memset(filename_xtm, 0, filename_len);
		
		strncpy(filename_xtm, filename_001_xtm, filename_len - 7);

		nbrstr = xtm_nbr_to_string(i);
		strcat(filename_xtm, nbrstr);
		free(nbrstr);
		
		strcat(filename_xtm, ".xtm");

		xtm_files[i-1] = filename_xtm;
		

	}

	xtm_files[nbr_of_files - 1] = NULL;
	
	return xtm_files;
}

void xtm_free_all_filenames(char **xtm_filenames)
{
	char **current;

	for (current = xtm_filenames; *current != NULL; current++)
		free(*current);
	free(xtm_filenames);

}

int xtm_assemble(struct xtm_info *xtm_info_struct, struct xtm_file **xtm_files_tab, char *new_file)
{
	FILE *fd;
	struct xtm_file **xtm_files_tab_2 = xtm_files_tab;
	int i;
	int nbr_of_files = 0;
	int readable_bytes;
	size_t size;
	void *buffer;
	
	if ((fd = fopen(new_file, "wb")) == NULL)
		return 1;
	else if (	xtm_files_tab == NULL
		|| 	new_file == NULL
		||	xtm_info_struct == NULL
		)
		return 1;
	

	if (xtm_info_struct->md5_hash_enabled == 1)
	{
		while (xtm_files_tab_2++)
			nbr_of_files++;
	}

	buffer = malloc(BLK_SIZE);


	for (i = 0; xtm_files_tab[i] != NULL; i++)
	{

		readable_bytes = xtm_files_tab[i]->filesize;

		if (i == 0)
		{
			readable_bytes -= 104;
			fseek(xtm_files_tab[i]->fd, 104, SEEK_SET);
		}
		else
		{
			fseek(xtm_files_tab[i]->fd, 0, SEEK_SET);
		}

		if (xtm_info_struct->md5_hash_enabled == 1 && (i+1) == nbr_of_files)
			readable_bytes -= nbr_of_files * 16;


		while (readable_bytes > 0)
		{

			if (readable_bytes > BLK_SIZE)
				size = BLK_SIZE;
			else
				size = readable_bytes;
			
			readable_bytes -= size;
			
			fread(buffer, size, 1, xtm_files_tab[i]->fd);
 			fwrite(buffer, size, 1, fd);
			
		}
	}

	free(buffer);

	return 0;
	
}
